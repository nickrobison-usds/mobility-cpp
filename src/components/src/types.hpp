//
// Created by Nicholas Robison on 6/9/20.
//

#ifndef MOBILITY_CPP_TYPES_HPP
#define MOBILITY_CPP_TYPES_HPP

#include <boost/bimap.hpp>

namespace components::detail {
    typedef boost::bimap<std::string, std::size_t> offset_bimap;
    typedef offset_bimap::value_type position;
}


#endif //MOBILITY_CPP_TYPES_HPP
