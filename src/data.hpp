//
// Created by Nicholas Robison on 5/19/20.
//

#ifndef MOBILITY_CPP_DATA_HPP
#define MOBILITY_CPP_DATA_HPP

#include <iostream>
#include <arrow/api.h>
#include <hpx/include/serialization.hpp>
#include <utility>

#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/fmt.h"

using ArrowDate = arrow::Date32Type::c_type;

struct data_row {
    const std::string location_cbg;
    const std::string visit_cbg;
    const ArrowDate date;
    std::vector<int16_t> visits;
    const double distance;

    template<typename OStream>
    friend OStream &
    operator<<(OStream &o, const data_row &dr) {
        auto msg = fmt::format("Location: {}\n", dr.location_cbg);
        return o << msg;
    }
};

struct visit_row {
    std::string location_cbg;
    std::string visit_cbg;
    ArrowDate date;
    int16_t visits;
    double distance;
    double weighted_total;

//    visit_row(std::string locationCbg, const std::string &visitCbg, const ArrowDate date, const int16_t visits,
//              const double distance, const double weightedTotal) : location_cbg(std::move(locationCbg)),
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
        using std::swap;

        swap(first.location_cbg, second.location_cbg);
        swap(first.visit_cbg, second.visit_cbg);
        swap(first.visits, second.visits);
        swap(first.date, second.date);
        swap(first.distance, second.distance);
        swap(first.weighted_total, second.weighted_total);
    }
};

#endif //MOBILITY_CPP_DATA_HPP
