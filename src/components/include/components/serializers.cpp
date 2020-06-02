//
// Created by Nicholas Robison on 6/2/20.
//

#include "serializers.hpp"

namespace hpx::serialization {
    void
    serialize(input_archive &ar, arrow::FieldPath &fp, unsigned int const) {
        ar & fp;
    }

    void
    serialize(output_archive &ar, arrow::FieldPath &fp, unsigned int const) {
        ar & fp;
    }
}