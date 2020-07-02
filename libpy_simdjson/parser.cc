#include <string>

#include <libpy/autoclass.h>
#include <libpy/automodule.h>

#include "simdjson.h"

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
};

using namespace std::string_literals;

LIBPY_AUTOMODULE(libpy_simdjson, simdjson, ({}))
(py::borrowed_ref<> m) {
    py::owned_ref t = py::autoclass<pysimdjson>(PyModule_GetName(m.get()) + ".Simdjson"s)
                          .new_<>()
                          .doc("Python bindings to Simdjson")  // add a class docstring
                          .def<&pysimdjson::load>("load")
                          .def<&pysimdjson::parse>("parse")
                          .type();
    return PyObject_SetAttrString(m.get(), "Simdjson", static_cast<PyObject*>(t));
}
}  // namespace libpy_simdjson
