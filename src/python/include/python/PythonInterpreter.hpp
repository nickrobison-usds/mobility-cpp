//
// Created by Nicholas Robison on 9/9/20.
//

#ifndef MOBILITY_CPP_PYTHONINTERPRETER_HPP
#define MOBILITY_CPP_PYTHONINTERPRETER_HPP

#include <string_view>
#include <pybind11/embed.h>
#include <pybind11/eval.h>

namespace py = pybind11;

namespace mcpp::python {

    class PythonInterpreter {
    public:
        explicit PythonInterpreter(): _interpreter({}) {
            py::module sys = py::module::import("sys");
            py::print(sys.attr("path"));

        }
        template <typename T>
        T evaluate(const std::string &v) {
            return py::eval(v).cast<T>();
        }

    private:
        const pybind11::scoped_interpreter _interpreter;
    };
}





#endif //MOBILITY_CPP_PYTHONINTERPRETER_HPP
