#include <string>

#include <libpy/autoclass.h>
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
class pysimdjson {
private:
    simdjson::dom::parser m_parser;
    simdjson::dom::element m_doc;

public:
    pysimdjson() = default;

    void load(std::string filename) {
        m_doc = m_parser.load(filename);
    }

    void loads(std::string in_string) {
        // TODO - padded?
        m_doc = m_parser.parse(in_string);
    }

    py::owned_ref <> at(std::string json_pntr) {
        simdjson::dom::element result;
        auto maybe_result = m_doc.at(json_pntr);
        auto error = maybe_result.get(result);
        if (error) {
            throw py::exception(PyExc_ValueError, "Could not get element at ", json_pntr);
        }
        return py::to_object(result);
    }
};

using namespace std::string_literals;

LIBPY_AUTOMODULE(libpy_simdjson, simdjson, ({}))
(py::borrowed_ref<> m) {
    py::owned_ref t = py::autoclass<pysimdjson>(PyModule_GetName(m.get()) + ".Simdjson"s)
                          .new_<>()
                          .doc("Python bindings to Simdjson")  // add a class docstring
                          .def<&pysimdjson::load>("load")
                          .def<&pysimdjson::loads>("loads")
                          .def<&pysimdjson::at>("at")
                          .type();
    return PyObject_SetAttrString(m.get(), "Simdjson", static_cast<PyObject*>(t));
}
}  // namespace lipy_simdjson
