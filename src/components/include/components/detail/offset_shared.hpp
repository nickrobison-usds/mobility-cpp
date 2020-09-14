//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_OFFSET_SHARED_HPP
#define MOBILITY_CPP_OFFSET_SHARED_HPP

#include <boost/bimap.hpp>
#include <string>

namespace components::detail {
    typedef boost::bimap<std::string, std::size_t> offset_bimap;
    typedef offset_bimap::value_type position;
}

#endif //MOBILITY_CPP_OFFSET_SHARED_HPP
