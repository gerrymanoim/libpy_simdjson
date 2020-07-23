#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>

#include <libpy/autoclass.h>
#include <libpy/autofunction.h>
#include <libpy/automodule.h>
#include <libpy/build_tuple.h>
#include <libpy/itertools.h>
#include <libpy/to_object.h>
#include <range/v3/all.hpp>

#include "simdjson.h"

namespace libpy_simdjson {
template<typename F>
decltype(auto) as_static_type(simdjson::dom::element el, F&& f) {
    switch (el.type()) {
    case simdjson::dom::element_type::ARRAY:
        return std::forward<F>(f)(simdjson::dom::array(el));
    case simdjson::dom::element_type::OBJECT:
        return std::forward<F>(f)(simdjson::dom::object(el));
    case simdjson::dom::element_type::INT64:
        return std::forward<F>(f)(int64_t(el));
    case simdjson::dom::element_type::UINT64:
        return std::forward<F>(f)(uint64_t(el));
    case simdjson::dom::element_type::DOUBLE:
        return std::forward<F>(f)(double(el));
    case simdjson::dom::element_type::STRING:
        return std::forward<F>(f)(std::string_view(el));
    case simdjson::dom::element_type::BOOL:
        return std::forward<F>(f)(bool(el));
    case simdjson::dom::element_type::NULL_VALUE:
        return std::forward<F>(f)(nullptr);
    default:
        throw py::exception(PyExc_ValueError, "Unexpected element_type encountered");
    }
}

}  // namespace libpy_simdjson

namespace py::dispatch {

template<>
struct to_object<simdjson::dom::array> : public sequence_to_object<simdjson::dom::array> {
};

template<>
struct to_object<simdjson::dom::object> : public map_to_object<simdjson::dom::object> {};

template<>
struct to_object<simdjson::dom::element> {
    static py::owned_ref<> f(const simdjson::dom::element& element) {
        return libpy_simdjson::as_static_type(element, [](auto el) {
            if constexpr (std::is_same_v<decltype(el), std::nullptr_t>) {
                return py::none;
            }
            else {
                return py::to_object(el);
            }
        });
    }
};

template<typename T>
struct to_object<simdjson::simdjson_result<T>> {
    static py::owned_ref<> f(const simdjson::simdjson_result<T>& maybe_result) {
        simdjson::dom::element result;
        auto error = maybe_result.get(result);
        if (error) {
            throw py::exception(PyExc_ValueError, simdjson::error_message(error));
        }
        return py::to_object(result);
    }
};

template<>
struct to_object<simdjson::dom::key_value_pair> {
    static py::owned_ref<> f(const simdjson::dom::key_value_pair& key_value_pair) {
        return py::build_tuple(key_value_pair.key, key_value_pair.value);
    }
};

}  // namespace py::dispatch

namespace libpy_simdjson {

template<typename T>
bool element_eq(T a, T b) {
    return a == b;
}

bool element_eq(simdjson::dom::object lhs, simdjson::dom::object rhs);

bool element_eq(simdjson::dom::array lhs, simdjson::dom::array rhs) {
    return lhs.size() == rhs.size() &&
           std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](auto a, auto b) {
               return as_static_type(a, [&](auto a_static) {
                   if constexpr (std::is_same_v<decltype(a_static), std::nullptr_t>) {
                       return b.type() == simdjson::dom::element_type::NULL_VALUE;
                   }
                   else {
                       decltype(a_static) b_static;
                       if (b.get(b_static)) {
                           return false;
                       }
                       return element_eq(a_static, b_static);
                   }
               });
           });
}

bool element_eq(simdjson::dom::object lhs, simdjson::dom::object rhs) {
    return lhs.size() == rhs.size() &&
           std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](auto a, auto b) {
               return a.key == b.key && as_static_type(a.value, [&](auto a_static) {
                          if constexpr (std::is_same_v<decltype(a_static),
                                                       std::nullptr_t>) {
                              return b.value.type() ==
                                     simdjson::dom::element_type::NULL_VALUE;
                          }
                          else {
                              decltype(a_static) b_static;
                              if (b.value.get(b_static)) {
                                  return false;
                              }
                              return element_eq(a_static, b_static);
                          }
                      });
           });
}

class parser : public std::enable_shared_from_this<parser> {
private:
    simdjson::dom::parser m_parser;

public:
    parser() = default;

    std::shared_ptr<parser> getptr() {
        return shared_from_this();
    }

    py::owned_ref<> load(const std::filesystem::path& filename);

    py::owned_ref<> loads(std::string_view in_string);

    static py::owned_ref<> load_method(const std::shared_ptr<parser>& p,
                                       const std::filesystem::path& filename) {
        return p->load(filename);
    }

    static py::owned_ref<> loads_method(const std::shared_ptr<parser>& p,
                                        std::string_view in_string) {
        return p->loads(in_string);
    }
};

class object_element {
private:
    std::shared_ptr<parser> m_parser;
    simdjson::dom::object m_value;

public:
    object_element(std::shared_ptr<parser> parser_pntr, simdjson::dom::object value)
        : m_parser(parser_pntr), m_value(value) {}

    py::owned_ref<> operator[](const std::string& field);

    py::owned_ref<> at(const std::string& json_pntr);

    py::owned_ref<> items() const {
        return py::dispatch::sequence_to_object<simdjson::dom::object>::f(m_value);
    }

private:
    auto keys_range() const {
        return m_value |
               ranges::views::transform([](const auto& item) { return item.key; });
    }

public:
    py::owned_ref<> keys() const {
        auto keys = keys_range();
        return py::dispatch::sequence_to_object<decltype(keys)>::f(keys);
    }

    py::owned_ref<> values() const {
        auto values = m_value | ranges::views::transform(
                                    [](const auto& item) { return item.value; });
        return py::dispatch::sequence_to_object<decltype(values)>::f(values);
    }

    py::owned_ref<> as_dict() const {
        return py::to_object(m_value);
    }

    std::size_t size() const {
        return m_value.size();
    }

    auto begin() const {
        return keys_range().begin();
    }

    auto end() const {
        return keys_range().end();
    }

    bool operator==(const object_element& other) {
        if (this->size() != other.size()) {
            return false;
        }

        return element_eq(this->m_value, other.m_value);
    }
};

class array_element {
private:
    std::shared_ptr<parser> m_parser;
    simdjson::dom::array m_value;

public:
    array_element(std::shared_ptr<parser> parser_pntr, simdjson::dom::array value)
        : m_parser(parser_pntr), m_value(value) {}

    py::owned_ref<> at(const std::string& json_pntr);

    py::owned_ref<> operator[](std::ptrdiff_t index);

    py::owned_ref<> as_list() {
        return py::to_object(m_value);
    }

    std::size_t size() const {
        return m_value.size();
    }

    typedef simdjson::dom::array::iterator iterator;

    iterator begin() const {
        return m_value.begin();
    }

    iterator end() const {
        return m_value.end();
    }

    bool operator==(const array_element& other) {
        if (this->size() != other.size()) {
            return false;
        }

        return element_eq(this->m_value, other.m_value);
    }
};

py::owned_ref<> disambiguate_result(std::shared_ptr<parser> parser_pntr,
                                    simdjson::dom::element result) {
    auto result_type = result.type();
    switch (result_type) {
    case simdjson::dom::element_type::ARRAY:
        return py::autoclass<array_element>::construct(parser_pntr, result);
    case simdjson::dom::element_type::OBJECT:
        return py::autoclass<object_element>::construct(parser_pntr, result);
    default:
        return py::to_object(result);
    }
}

py::owned_ref<> parser::load(const std::filesystem::path& filename) {
    if (weak_from_this().use_count() > 1) {
        throw py::exception(PyExc_ValueError,
                            "cannot reparse while live objects exist from a prior parse");
    }
    auto maybe_result = m_parser.load(filename.string());
    simdjson::dom::element result;
    auto error = maybe_result.get(result);
    if (error) {
        throw py::exception(PyExc_ValueError, simdjson::error_message(error));
    }
    return disambiguate_result(shared_from_this(), result);
}

py::owned_ref<> parser::loads(std::string_view in_string) {
    if (weak_from_this().use_count() > 1) {
        throw py::exception(PyExc_ValueError,
                            "cannot reparse while live objects exist from a prior parse");
    }
    auto maybe_result = m_parser.parse(in_string.data(), in_string.size());
    simdjson::dom::element result;
    auto error = maybe_result.get(result);
    if (error) {
        throw py::exception(PyExc_ValueError, simdjson::error_message(error));
    }
    return disambiguate_result(shared_from_this(), result);
}

py::owned_ref<> object_element::operator[](const std::string& field) {
    return disambiguate_result(m_parser, m_value[field]);
}

py::owned_ref<> object_element::at(const std::string& json_pntr) {
    simdjson::dom::element result;
    auto maybe_result = m_value.at(json_pntr);
    auto error = maybe_result.get(result);
    if (error) {
        throw py::exception(PyExc_KeyError, json_pntr);
    }
    return disambiguate_result(m_parser, result);
}

py::owned_ref<> array_element::at(const std::string& json_pntr) {
    simdjson::dom::element result;
    auto maybe_result = m_value.at(json_pntr);
    auto error = maybe_result.get(result);
    if (error) {
        throw py::exception(PyExc_IndexError, json_pntr);
    }
    return disambiguate_result(m_parser, result);
}

py::owned_ref<> array_element::operator[](std::ptrdiff_t index) {
    std::ptrdiff_t original_index = index;
    if (index < 0) {
        index += m_value.size();
    }

    simdjson::dom::element result;
    auto maybe_result = m_value.at(index);
    auto error = maybe_result.get(result);
    if (error) {
        throw py::exception(PyExc_IndexError, original_index);
    }
    return disambiguate_result(m_parser, result);
}

py::owned_ref<> load(const std::filesystem::path& filename) {
    return std::make_shared<parser>()->load(filename);
}

py::owned_ref<> loads(const std::string& in_string) {
    return std::make_shared<parser>()->loads(in_string);
}

py::owned_ref<> __simdjson_version__() {
    return py::to_object(STRINGIFY(SIMDJSON_VERSION));
}

LIBPY_AUTOMODULE(libpy_simdjson,
                 parser,
                 ({py::autofunction<load>("load"),
                   py::autofunction<loads>("loads"),
                   py::autofunction<__simdjson_version__>("__simdjson_version__")}))
(py::borrowed_ref<> m) {
    py::autoclass<std::shared_ptr<parser>>(m, "Parser")
        .new_<std::make_shared<parser>>()
        .doc("Base parser")  // add a class docstring
        .def<&parser::load_method>("load")
        .def<&parser::loads_method>("loads")
        .type();
    py::autoclass<object_element>(m, "Object")
        .mapping<std::string>()
        .def<&object_element::at>("at")
        .def<&object_element::as_dict>("as_dict")
        .def<&object_element::keys>("keys")
        .def<&object_element::values>("values")
        .def<&object_element::items>("items")
        .comparisons<object_element>()
        .len()
        .iter()
        .type();
    py::autoclass<array_element>(m, "Array")
        .def<&array_element::at>("at")
        .def<&array_element::as_list>("as_list")
        .mapping<std::ptrdiff_t>()
        .comparisons<array_element>()
        .len()
        .iter()
        .type();

    return false;
}
}  // namespace libpy_simdjson
