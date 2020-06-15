//
// Created by Nicholas Robison on 6/15/20.
//

#ifndef MOBILITY_CPP_TYPE_HELPERS_HPP
#define MOBILITY_CPP_TYPE_HELPERS_HPP

namespace shared::detail {

    constexpr bool debugEnabled() {
#ifdef __APPLE__
        return true;
#else
        return false;
#endif
    }


}

#endif //MOBILITY_CPP_TYPE_HELPERS_HPP
