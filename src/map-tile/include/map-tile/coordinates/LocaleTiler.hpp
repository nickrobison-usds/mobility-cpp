//
// Created by Nicholas Robison on 7/27/20.
//

#ifndef MOBILITY_CPP_LOCALETILER_HPP
#define MOBILITY_CPP_LOCALETILER_HPP

#include <vector>
#include <array>
#include "LocaleLocator.hpp"

namespace mt::coordinates {

    class LocaleTiler {
    public:
        template<class Coordinate>
        static std::vector<std::pair<bg::model::box<Coordinate>, std::uint64_t>> tile(const Coordinate &min, const Coordinate &max, const std::array<std::size_t, 2> &stride);
    };
}



#endif //MOBILITY_CPP_LOCALETILER_HPP
