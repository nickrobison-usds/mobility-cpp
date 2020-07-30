//
// Created by Nicholas Robison on 7/29/20.
//

#ifndef MOBILITY_CPP_TRAITS_HPP
#define MOBILITY_CPP_TRAITS_HPP

#include "map-tile/ctx/Context.hpp"
#include <type_traits>

/**
 * Simple trait helper to determine if the given class has a setup() method.
 * If so, we'll call it once, immediately after Mapper creation
 *
 */
template<typename, typename, typename, typename = void>
struct has_setup : std::false_type {
};

template<typename T, typename Key, typename Coordinate>
struct has_setup<T, Key, Coordinate, std::void_t<decltype(&T::setup)>>
        : std::is_same<void, decltype(std::declval<T>().setup(std::declval<mt::ctx::Context<Key, Coordinate>>()))> {
};


#endif //MOBILITY_CPP_TRAITS_HPP
