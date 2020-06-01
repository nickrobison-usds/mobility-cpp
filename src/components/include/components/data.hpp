//
// Created by Nicholas Robison on 5/19/20.
//

#ifndef MOBILITY_CPP_DATA_HPP
#define MOBILITY_CPP_DATA_HPP

#include <iostream>
#include <arrow/api.h>
#include <hpx/include/serialization.hpp>
#include <utility>
#include "ogr_geometry.h"

#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/fmt.h"

using namespace std;

using ArrowDate = arrow::Date32Type::c_type;

struct data_row {
    const string location_cbg;
    const string visit_cbg;
    const ArrowDate date;
    vector<int16_t> visits;
    const double distance;

    template<typename OStream>
    friend OStream &
    operator<<(OStream &o, const data_row &dr) {
        auto msg = fmt::format("Location: {}\n", dr.location_cbg);
        return o << msg;
    }
};

struct visit_row {
    string location_cbg;
    string visit_cbg;
    ArrowDate date;
    int16_t visits;
    double distance;
    double weighted_total;

//    visit_row(string locationCbg, const string &visitCbg, const ArrowDate date, const int16_t visits,
//              const double distance, const double weightedTotal) : location_cbg(move(locationCbg)),
//                                                                   visit_cbg(visitCbg),
//                                                                   date(date), visits(visits), distance(distance) {};
//
//    visit_row(visit_row &&other) noexcept: visit_row() {
//        swap(*this, other);
//    }
    // Serialization support: even if all of the code below runs on one
    // locality only, we need to provide an (empty) implementation for the
    // serialization as all arguments passed to actions have to support this.
    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) const {
    }

    friend void swap(visit_row &first, visit_row& second) {

        swap(first.location_cbg, second.location_cbg);
        swap(first.visit_cbg, second.visit_cbg);
        swap(first.visits, second.visits);
        swap(first.date, second.date);
        swap(first.distance, second.distance);
        swap(first.weighted_total, second.weighted_total);
    }
};

struct joined_location {
    string safegraph_place_id;
    double latitude;
    double longitude;
    uint64_t location_cbg;
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
    OGRPoint location;
    string cbg;
//    double latitude;
//    double longitude;
//    string street_address;
//    string city;
//    string region;
//    uint32_t postal_code;
//    string iso_country_code;
//    string phone_number;
//    string open_hours;
//    string category_tags;

    // Serialization support: even if all of the code below runs on one
    // locality only, we need to provide an (empty) implementation for the
    // serialization as all arguments passed to actions have to support this.
    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) const {
        std::string location_wkt;
        auto wkt_char = location_wkt.data();
        location.exportToWkt(&wkt_char);
        location_wkt.length();
//        ar & safegraph_place_id
//        & parent_safegraph_place_id
//        & location_name
//        & safegraph_brand_ids
//        & brands
//        & top_category
//        & sub_category
//        & naics_code
//        & location_wkt
//        & cbg;
    }
};
// Add some custom serializers
namespace hpx::serialization {
    template <typename Archive>
    void serialize(Archive& ar, ArrowDate& date, unsigned int const)
    {
        ar & date;
    }

    // explicit instantiation for the correct archive types
    template HPX_COMPONENT_EXPORT void
    serialize(input_archive&, ArrowDate&, unsigned int const);
    template HPX_COMPONENT_EXPORT void
    serialize(output_archive&, ArrowDate&, unsigned int const);
}

#endif //MOBILITY_CPP_DATA_HPP
