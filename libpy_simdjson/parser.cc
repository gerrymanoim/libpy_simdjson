#include <memory>
#include <string>

#include <libpy/autoclass.h>
#include <libpy/autofunction.h>
#include <libpy/automodule.h>
#include <libpy/to_object.h>

#include "simdjson.h"

namespace py::dispatch {

template<>
struct to_object<simdjson::dom::array> {
    static py::owned_ref<> f(const simdjson::dom::array& v) {
        return sequence_to_list(v);
    }
};

template<>
struct to_object<simdjson::dom::object> : public map_to_object<simdjson::dom::object> {};

template<>
struct to_object<simdjson::dom::element> {
    static py::owned_ref<> f(const simdjson::dom::element& element) {
        auto element_type = element.type();
        switch (element_type) {
        case simdjson::dom::element_type::ARRAY:
            return py::to_object(simdjson::dom::array(element));
        case simdjson::dom::element_type::OBJECT:
            return py::to_object(simdjson::dom::object(element));
        case simdjson::dom::element_type::INT64:
            return py::to_object(int64_t(element));
        case simdjson::dom::element_type::UINT64:
            return py::to_object(uint64_t(element));
        case simdjson::dom::element_type::DOUBLE:
            return py::to_object(double(element));
        case simdjson::dom::element_type::STRING:
            return py::to_object(std::string_view(element));
        case simdjson::dom::element_type::BOOL:
            return py::to_object(bool(element));
        case simdjson::dom::element_type::NULL_VALUE:
            return py::none;
        default:
            throw py::exception(PyExc_ValueError, "Unexpected element_type encountered");
        }
    }
};

template<typename T>
struct to_object<simdjson::simdjson_result<T>> {
    static py::owned_ref<> f(const simdjson::simdjson_result<T>& maybe_result) {
        simdjson::dom::element result;
        auto error = maybe_result.get(result);
        if (error) {
            // throw
            throw py::exception(PyExc_ValueError, "Issue with that parse");
        }
        return py::to_object(result);
    }
};

}  // namespace py::dispatch

namespace libpy_simdjson {

class parser;
class array_element;

class object_element {
private:
    std::shared_ptr<parser> m_parser;
    simdjson::dom::object m_value;

public:
    object_element(std::shared_ptr<libpy_simdjson::parser> parser_pntr, simdjson::dom::object value) {
        m_parser = parser_pntr;
        m_value = value;
    }

    ~object_element() {
        m_parser->decref();
    }

    py::owned_ref<> disambiguate_result(simdjson::dom::element result) {
        auto result_type = result.type();
        switch (result_type) {
        case simdjson::dom::element_type::ARRAY:
            return array_element(m_parser, result);
        case simdjson::dom::element_type::OBJECT:
            return object_element(m_parser, result);
        default:
            return py::to_object(result);
        }
    }

    py::owned_ref<> at(std::string json_pntr) {
        simdjson::dom::element result;
        auto maybe_result = m_value.at(json_pntr);
        auto error = maybe_result.get(result);
        if (error) {
            throw py::exception(PyExc_ValueError, "Could not get element at ", json_pntr);
        }
        return disambiguate_result(result);
    }

    using iterator = simdjson::dom::object::iterator;

    iterator begin() {
        return m_value.begin();
    }

    iterator end() {
        return m_value.end();
    }
    // TODO keys, values, etc
};

class array_element {
private:
    std::shared_ptr<parser> m_parser;
    simdjson::dom::array m_value;

public:
    array_element(std::shared_ptr<libpy_simdjson::parser> parser_pntr, simdjson::dom::object value) {
        m_parser = parser_pntr;
        m_value = value;
    }

    ~array_element() {
        m_parser->decref();
    }

    py::owned_ref<> disambiguate_result(simdjson::dom::element result) {
        auto result_type = result.type();
        switch (result_type) {
        case simdjson::dom::element_type::ARRAY:
            return array_element(m_parser, result);
        case simdjson::dom::element_type::OBJECT:
            return object_element(m_parser, result);
        default:
            return py::to_object(result);
        }
    }

    py::owned_ref<> at(std::string json_pntr) {
        simdjson::dom::element result;
        auto maybe_result = m_value.at(json_pntr);
        auto error = maybe_result.get(result);
        if (error) {
            throw py::exception(PyExc_ValueError, "Could not get element at ", json_pntr);
        }
        return disambiguate_result(result);
    }

    using iterator = simdjson::dom::array::iterator;

    iterator begin() {
        return m_value.begin();
    }

    iterator end() {
        return m_value.end();
    }
};

class parser : std::enable_shared_from_this<parser> {
private:
    simdjson::dom::parser m_parser;

public:
    parser() = default;

    std::shared_ptr<parser> getptr() {
        return shared_from_this();
    }

    py::owned_ref<> load(std::string filename) {
        if (weak_from_this().use_count() > 1) {
            throw py::exception(
                PyExc_ValueError,
                "cannot reparse while live objects exist from a prior parse");
        }
        auto m_doc = m_parser.load(filename);
        return disambiguate_result(m_doc);
    }

    py::owned_ref<> loads(std::string in_string) {
        if (weak_from_this().use_count() > 1) {
            throw py::exception(
                PyExc_ValueError,
                "cannot reparse while live objects exist from a prior parse");
        }
        auto m_doc = m_parser.parse(in_string);
        return disambiguate_result(m_doc);
    }

    py::owned_ref<> disambiguate_result(simdjson::dom::element result) {
        auto result_type = result.type();
        switch (result_type) {
        case simdjson::dom::element_type::ARRAY:
            return array_element(shared_from_this(), result);
        case simdjson::dom::element_type::OBJECT:
            return object_element(shared_from_this(), result);
        default:
            return py::to_object(result);
        }
    }
};

py::owned_ref<> load(std::string filename) {
    parser this_parser();
    return this_parser.load(filename);
}

py::owned_ref<> loads(std::string in_string) {
    parser this_parser();
    return this_parser.loads(in_string);
}

using namespace std::string_literals;

LIBPY_AUTOMODULE(libpy_simdjson,
                 simdjson,
                 ({py::autofunction<load>("load"), py::autofunction<loads>("loads")}))
(py::borrowed_ref<> m) {
    py::owned_ref a = py::autoclass<parser>(PyModule_GetName(m.get()) + ".Parser"s)
                          .new_<>()
                          .doc("Base parser")  // add a class docstring
                          .def<&parser::load>("load")
                          .def<&parser::loads>("loads")
                          .type();
    PyObject_SetAttrString(m.get(), "Parser", static_cast<PyObject*>(a));
    py::owned_ref b = py::autoclass<object_element>(PyModule_GetName(m.get()) +
                                                    ".Object"s)
                          .def<&object_element::at>("at")
                          .type();
    PyObject_SetAttrString(m.get(), "Object", static_cast<PyObject*>(b));

    py::owned_ref c = py::autoclass<array_element>(PyModule_GetName(m.get()) + ".Array"s)
                          .def<&array_element::at>("at")
                          .type();
    PyObject_SetAttrString(m.get(), "Array", static_cast<PyObject*>(c));

    return true;
}
}  // namespace libpy_simdjson
