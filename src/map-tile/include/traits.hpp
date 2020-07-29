//
// Created by Nicholas Robison on 7/29/20.
//

#ifndef MOBILITY_CPP_TRAITS_HPP
#define MOBILITY_CPP_TRAITS_HPP

#include <type_traits>

/**
 * Simple trait helper to determine if the given class has a setup() method.
 * If so, we'll call it once, immediately after Mapper creation
 */
template<typename, typename = void>
struct has_setup : std::false_type {
};

template<typename T>
struct has_setup<T, std::void_t<decltype(&T::setup)>> : std::is_same<void, decltype(std::declval<T>().setup())> {
};

#endif //MOBILITY_CPP_TRAITS_HPP
