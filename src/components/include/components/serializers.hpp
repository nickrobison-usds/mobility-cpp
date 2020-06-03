//
// Created by Nicholas Robison on 6/2/20.
//

#ifndef MOBILITY_CPP_SERIALIZERS_HPP
#define MOBILITY_CPP_SERIALIZERS_HPP

#include <arrow/api.h>
#include <ogr_geometry.h>
#include <hpx/serialization/serialize.hpp>

/**
 * Collection of custom serializers to make HPX happy.
 */
namespace hpx::serialization {

    void
    serialize(input_archive &ar, arrow::FieldPath &fp, unsigned int const);

    void
    serialize(output_archive &ar, arrow::FieldPath &fp, unsigned int const);

    void
    serialize(input_archive &ar, OGRPoint &fp, unsigned int const);

    void
    serialize(output_archive &ar, OGRPoint &fp, unsigned int const);
}

#endif //MOBILITY_CPP_SERIALIZERS_HPP
