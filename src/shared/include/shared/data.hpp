//
// Created by Nicholas Robison on 5/19/20.
//

#ifndef MOBILITY_CPP_DATA_HPP
#define MOBILITY_CPP_DATA_HPP

#include <chrono>
#include <iostream>
#include <utility>
#include "serializers.hpp"
#include "ogr_geometry.h"
#include <date/date.h>

#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/fmt.h"

using namespace std;

struct data_row {
    string location_cbg;
    string visit_cbg;
    date::sys_days date;
    vector<uint32_t> visits;
    double distance;

    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & location_cbg & visit_cbg & date & visits & distance;
    }

    template<typename OStream>
    friend OStream &
    operator<<(OStream &o, const data_row &dr) {
        auto msg = fmt::format("Location: {}\n", dr.location_cbg);
        return o << msg;
    }
};

struct weekly_pattern {
    string safegraph_place_id;
    string location_name;
    date::sys_days date_range_start;
    date::sys_days date_range_end;
    uint32_t raw_visit_counts;
    uint32_t raw_visitor_counts;
    string visits_by_day;
    string visits_by_each_hour;
    uint64_t poi_cbg;
    string visitor_home_cbgs;
};

struct visit_row {
    string location_cbg;
    string visit_cbg;
    date::sys_days date;
    uint32_t visits;
    double distance;
    double weighted_total;

    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & location_cbg & visit_cbg & date & visits & distance & weighted_total;
    }
};

struct v2 {
    string safegraph_place_id;
    date::sys_days visit_date;
    string location_cbg;
    string visit_cbg;
    uint32_t visits;
    double distance;
    double weighted_total;

    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & safegraph_place_id & visit_date & location_cbg & visit_cbg & visits & distance & weighted_total;
    }

    bool operator==(const v2 &rhs) const {
        return safegraph_place_id == rhs.safegraph_place_id
               && location_cbg == rhs.location_cbg
               && visit_cbg == rhs.visit_cbg
               && visits == rhs.visits
               && distance == rhs.distance
               && weighted_total == rhs.weighted_total;
    }
};

struct joined_location {
    string safegraph_place_id;
    double latitude;
    double longitude;
    string location_cbg;

    // Serialization support: even if all of the code below runs on one
    // locality only, we need to provide an (empty) implementation for the
    // serialization as all arguments passed to actions have to support this.
    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & safegraph_place_id & latitude & longitude & location_cbg;
    }
};

struct safegraph_location {
    string safegraph_place_id;
    string parent_safegraph_place_id;
    string location_name;
    string safegraph_brand_ids;
    string brands;
    string top_category;
    string sub_category;
    string naics_code;
    double latitude;
    double longitude;
    string cbg;

    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar
        & safegraph_place_id
        & parent_safegraph_place_id
        & location_name
        & safegraph_brand_ids
        & brands
        & top_category
        & sub_category
        & naics_code
        & latitude
        & longitude
        & cbg;
    }
};

#endif //MOBILITY_CPP_DATA_HPP
