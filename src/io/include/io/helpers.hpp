//
// Created by Nicholas Robison on 7/13/20.
//

#ifndef MOBILITY_CPP_HELPERS_HPP
#define MOBILITY_CPP_HELPERS_HPP

#include <iostream>

#endif //MOBILITY_CPP_HELPERS_HPP

namespace io::helpers {
    inline int64_t stringSize(std::size_t maxSize = H5T_VARIABLE) {
        auto string_type = H5Tcopy(H5T_C_S1);
        const auto err = H5Tset_size(string_type, maxSize);
        // TODO: Get rid of this
        if (err) {
            std::cout << err << std::endl;
        }
        return string_type;
    }
}