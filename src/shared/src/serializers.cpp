//
// Created by Nicholas Robison on 6/2/20.
//

#include "shared/serializers.hpp"

namespace hpx::serialization {
    void
    serialize(input_archive &ar, arrow::FieldPath &fp, unsigned int const) {
        ar & fp;
    }

    void
    serialize(output_archive &ar, arrow::FieldPath &fp, unsigned int const) {
        ar & fp;
    }

    void
    serialize(input_archive &ar, OGRPoint &fp, unsigned int const) {
        double x;
        double y;
        ar & x & y;
        fp.setX(x);
        fp.setY(y);
    }

    void
    serialize(output_archive &ar, OGRPoint &fp, unsigned int const) {
        double x = fp.getX();
        double y = fp.getY();
        ar & x & y;
    }

    void serialize(input_archive &ar, date::sys_days &dt, unsigned int const) {
        std::uint64_t count;
        ar & count;
        dt = date::sys_days{} + date::days{count};
    }

    void serialize(output_archive &ar, date::sys_days &dt, unsigned int const) {
        auto count = dt.time_since_epoch().count();
        ar & count;
    }
}