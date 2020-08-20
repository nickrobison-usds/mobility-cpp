//
// Created by Nicholas Robison on 8/20/20.
//

#ifndef MOBILITY_CPP_HASHER_HPP
#define MOBILITY_CPP_HASHER_HPP

#include <boost/graph/adjacency_list.hpp>
#include <utility>

namespace boost::detail {
    /**
     * Abseil hash implementation for Boost edge descripton
     * @tparam H - Hash implementation
     * @tparam Tag - Boost graph type param
     * @tparam Value - property value
     * @param h - Current hash
     * @param edge - Edge to hash
     * @return - Hash value
     */
    template <typename H, typename Tag, typename Value>
    H AbslHashValue(H h, const edge_desc_impl<Tag, Value> &edge) {
        return H::combine(std::move(h), edge.m_eproperty, edge.m_source, edge.m_target);
    }
}

#endif //MOBILITY_CPP_HASHER_HPP
