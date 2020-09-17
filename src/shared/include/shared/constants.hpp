//
// Created by Nicholas Robison on 6/9/20.
//

#ifndef MOBILITY_CPP_CONSTANTS_HPP
#define MOBILITY_CPP_CONSTANTS_HPP

#include <cstdlib>

namespace shared {
    // The total number of Census Block Groups (CBGs) in the US
    const static std::size_t MAX_CBG = 220740;
    // Total number of counties in the US
    const static std::size_t MAX_COUNTY = 3226;

    enum SignPostCode {
        EXPAND_ROW,
        INSERT_ROWS,
        COMPUTE_DISTANCES,
        GET_CENTROIDS,
    };
}

#endif //MOBILITY_CPP_CONSTANTS_HPP
