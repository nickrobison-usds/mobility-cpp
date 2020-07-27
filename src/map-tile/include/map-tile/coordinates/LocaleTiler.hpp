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
        static std::vector<LocaleLocator::value> tile(const Coordinate2D &min, const Coordinate2D &max, const std::array<std::size_t, 2> &stride);
    };
}



#endif //MOBILITY_CPP_LOCALETILER_HPP
