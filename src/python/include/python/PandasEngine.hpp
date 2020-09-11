//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_PANDASENGINE_HPP
#define MOBILITY_CPP_PANDASENGINE_HPP

#define FORCE_IMPORT_ARRAY

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
        explicit PandasEngine(const std::size_t length) {
            // Not used
        };

        void load(const T &value) {
            hana::for_each(hana::accessors<T>(), [this, &value](auto pair) {
                auto key = hana::first(pair);
                auto &vec = hana::at_key(_internal, hana::first(pair));
                auto v = hana::second(pair)(value);
                vec.push_back(v);
                std::cout << key.c_str() << " " << v << std::endl;
            });
        }

        std::string evaluate() const {
            return std::to_string(hana::at_key(_internal, BOOST_HANA_STRING("name")).size());
        }

    private:
        using A = decltype(detail::type_map<T>());
        A _internal;
    };
}

#endif //MOBILITY_CPP_PANDASENGINE_HPP
