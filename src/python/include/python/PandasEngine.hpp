//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_PANDASENGINE_HPP
#define MOBILITY_CPP_PANDASENGINE_HPP

#include "pybind11/pybind11.h"
#define FORCE_IMPORT_ARRAY

#include "PythonInterpreter.hpp"
#include "helpers.hpp"
#include <boost/hana.hpp>
#include <xtensor-python/pyarray.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace hana = boost::hana;
using namespace hana::literals;

namespace mcpp::python {

    template<typename T>
    class PandasEngine {
    public:
        explicit PandasEngine(const std::size_t length = 0): _interpreter() {
            if (length > 0) {
                hana::for_each(hana::values(_internal), [&length](auto vec) {
                    vec.reserve(length);
                });
            }
        };

        void load(const T &value) {
            hana::for_each(hana::accessors<T>(), [this, &value](auto pair) {
                auto &vec = hana::at_key(_internal, hana::first(pair));
                auto v = hana::second(pair)(value);
                vec.push_back(v);
            });
        }

        std::string evaluate() const {

            // Adapt our vectors into tensors and prepare to load them
            auto entries = hana::front(hana::values(_internal)).size();

            // Create the index array
            auto indicies = xt::pyarray<std::size_t>::from_shape({2, 3, 2});
//            std::iota(indicies.begin(), indicies.end(), 0);


            return std::to_string(indicies.size());
        }

    private:
        using A = decltype(detail::type_map<T>());
        A _internal;
        PythonInterpreter _interpreter;
    };
}

PYBIND11_MODULE(pandas_engine, m) {
    xt::import_numpy();

    m.doc() = "Map-tile Python engine";

//    py::class_<mcpp::python::PandasEngine>(c)
}

#endif //MOBILITY_CPP_PANDASENGINE_HPP
