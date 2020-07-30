//
// Created by Nicholas Robison on 7/30/20.
//

#ifndef MOBILITY_CPP_CONVERSIONUTILS_HPP
#define MOBILITY_CPP_CONVERSIONUTILS_HPP

#include <string>

namespace shared {
    class ConversionUtils {
    public:
        template<typename Num>
        static Num convert_empty(const std::string &val);
    };
}


#endif //MOBILITY_CPP_CONVERSIONUTILS_HPP
