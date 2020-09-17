//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_PANDASENGINE_HPP
#define MOBILITY_CPP_PANDASENGINE_HPP

#include "PythonInterpreter.hpp"
#include "helpers.hpp"
#include <absl/strings/str_split.h>
#include <boost/hana.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace hana = boost::hana;
using namespace hana::literals;
namespace py = pybind11;
using namespace pybind11::literals;

namespace mcpp::python {

    std::once_flag imported;

    template<typename T>
    class PandasEngine {
    public:
        explicit PandasEngine(const std::string_view import_path, const std::size_t length = 1): _interpreter(), _import_path(import_path) {
            // Initialize xtensor and pandas support
//            std::call_once(imported, []() {
//                spdlog::debug("Initializing XT Numpy");
//                xt::import_numpy();
//            });

            if (length > 0) {
                hana::for_each(hana::values(_data), [&length](auto vec) {
                    vec.reserve(length);
                });
            }
        };

        void load(const T &value) {
            hana::for_each(hana::accessors<T>(), [this, &value](auto pair) {
                auto &vec = hana::at_key(_data, hana::first(pair));
                auto v = hana::second(pair)(value);
                vec.push_back(v);
            });
        }

        std::string evaluate() const {
            // Create a new Python dictionary for the results
            auto input_dict = py::dict();
            hana::for_each(_data, [&input_dict](auto entry) {
                auto key = hana::first(entry).c_str();
                spdlog::debug("Loading column: {}", key);
                input_dict[key] = hana::second(entry);
            });
            py::module sys = py::module::import("sys");
            py::print(sys.attr("path"));

            // Try to import the file
            auto pkg = py::module::import(_import_path.data());

            // Load pandas and create the dataframe
            auto pandas = py::module::import("pandas");
            auto args = py::make_tuple(input_dict);

            auto df = pandas.attr("DataFrame").attr("from_records")(*args);


            pkg.attr("compute")(df);

            return "12";
        }

    private:
        using A = decltype(detail::type_map<T>());
        A _data;
        PythonInterpreter _interpreter;
        const std::string_view _import_path;
    };
}

#endif //MOBILITY_CPP_PANDASENGINE_HPP
