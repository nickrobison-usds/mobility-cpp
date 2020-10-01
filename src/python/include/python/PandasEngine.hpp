//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_PANDASENGINE_HPP
#define MOBILITY_CPP_PANDASENGINE_HPP

#include "helpers.hpp"
#include <absl/strings/str_split.h>
#include <boost/hana.hpp>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace hana = boost::hana;
using namespace hana::literals;
namespace py = pybind11;
using namespace pybind11::literals;

namespace mcpp::python {
    std::once_flag imported;

    template<typename R>
    std::vector<R> unwrap_pandas(const py::object &obj) {
        const py::array result_arry = obj.attr("to_dict")("records");
        const ssize_t result_size = result_arry.size();
        spdlog::debug("Converting {} results", result_size);
        std::vector<R> results;
        results.reserve(result_size);
        for (const auto &res : result_arry) {
            const auto dict = res.cast<py::dict>();
            // Create the result value
            R tmp_result{};
            hana::for_each(hana::keys(tmp_result), [&tmp_result, &dict](auto key) {
                auto &member = hana::at_key(tmp_result, key);
                using Member = std::remove_reference_t<decltype(member)>;
                const Member v = dict[hana::to<char const *>(key)].template cast<Member>();
                member = v;
            });
            results.push_back(std::move(tmp_result));
        }

        return results;
    }

    template<typename T, class R = void>
    class PandasEngine {
    public:
        explicit PandasEngine(std::string import_path, const std::size_t length = 1) : _import_path(
                std::move(import_path)), _guard({}) {
            spdlog::info("Creating Pandas Engine");
            py::module sys = py::module::import("sys");
            py::print(sys.attr("path"));

            // Check for Pandas
            try {
                auto pandas = py::module::import("pandas");
            } catch (const std::exception &e) {
                spdlog::error("Cannot find installed Pandas");
                throw e;
            }

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

        template<typename Q = R>
        std::enable_if_t<std::is_same_v<Q, void>, void>
        evaluate() const {
            // Try to import the file
            spdlog::info("Loading Python module: {}", _import_path);
            auto pkg = py::module::import(_import_path.data());

            // Create a new Python dictionary for the results
            auto input_dict = py::dict();
            hana::for_each(_data, [&input_dict](auto entry) {
                auto key = hana::first(entry).c_str();
                spdlog::debug("Loading column: {}", key);
                input_dict[key] = hana::second(entry);
            });

            // Load pandas and create the dataframe
            auto pandas = py::module::import("pandas");
            auto args = py::make_tuple(input_dict);

            auto df = pandas.attr("DataFrame").attr("from_records")(*args);
            pkg.attr("compute")(df);
        }

        template<typename Q = R>
        [[nodiscard]]
        std::enable_if_t<!std::is_same_v<Q, void>, std::vector<Q>>
        evaluate() const {
            // Try to import the file
            spdlog::info("Loading Python module: {}", _import_path);
            auto pkg = py::module::import(_import_path.data());

            // Create a new Python dictionary for the results
            auto input_dict = py::dict();
            hana::for_each(_data, [&input_dict](auto entry) {
                auto key = hana::first(entry).c_str();
                spdlog::debug("Loading column: {}", key);
                input_dict[key] = hana::second(entry);
            });

            // Load pandas and create the dataframe
            auto pandas = py::module::import("pandas");
            auto args = py::make_tuple(input_dict);

            auto df = pandas.attr("DataFrame").attr("from_records")(*args);
            // Convert the result from a py:array of py::dict to a vector of the given result type
            auto return_df = pkg.attr("compute")(df);
            return unwrap_pandas<R>(return_df);
        }

    private:
        using A = decltype(detail::type_map<T>());
        A _data;
        const std::string _import_path;
        [[maybe_unused]] py::scoped_interpreter _guard;
    };
}

#endif //MOBILITY_CPP_PANDASENGINE_HPP
