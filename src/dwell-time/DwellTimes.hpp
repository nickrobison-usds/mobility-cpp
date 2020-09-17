//
// Created by Nicholas Robison on 9/17/20.
//

#ifndef MOBILITY_CPP_DWELLTIMES_HPP
#define MOBILITY_CPP_DWELLTIMES_HPP

#include <boost/hana.hpp>
#include <hpx/serialization/serialize.hpp>
#include <string>
#include <vector>

struct dwell_times {
    std::string safegraph_place_id;
    std::string visit_date;
    std::uint32_t visit_counts;
    float median_dwell;
    std::vector<std::uint16_t> dwell_array;

    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & safegraph_place_id & visit_date & visit_counts & median_dwell & dwell_array;
    }
};

BOOST_HANA_ADAPT_STRUCT(dwell_times, safegraph_place_id, visit_date, visit_counts, median_dwell, dwell_array);

#endif //MOBILITY_CPP_DWELLTIMES_HPP
