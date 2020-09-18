//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_HELPERS_HPP
#define MOBILITY_CPP_HELPERS_HPP

#include <boost/hana.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace hana = boost::hana;

namespace mcpp::python::detail {

    template<typename T>
    constexpr auto names = decltype(hana::unpack(std::declval<T>(), hana::on(hana::make_tuple, hana::first))) {};

    template<typename T>
    constexpr auto types = decltype(hana::unpack(std::declval<T>(), hana::on(hana::make_tuple, hana::compose(hana::typeid_, hana::second)))) {};

    template<typename T>
    constexpr auto types_as_vector() {
        return hana::transform(types<T>, hana::template_<std::vector>);
    }

    template<typename T>
    constexpr auto types_to_pyarray() {
        return hana::transform(types<T>, [](auto typ) {
            using inner = typename decltype(typ)::type;
            return hana::traits::declval(hana::type_c<std::vector<inner>>);
        });
    }

    template<typename T>
    constexpr auto type_map() {
        auto types = types_to_pyarray<T>();
        auto p = hana::zip_with([](auto name, auto value) {
            return hana::make_pair(name, value);
        }, names<T>, types);

        return hana::to_map(p);
    }
};

#endif //MOBILITY_CPP_HELPERS_HPP
