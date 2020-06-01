//
// Created by Nicholas Robison on 6/1/20.
//

#include "TileDimension.hpp"

namespace hpx::serialization {
    template <typename Archive>
    void serialize(Archive& ar, components::TileDimension& dim, unsigned int const)
    {
        ar & dim._cbg_offset & dim._cbg_count & dim._time_offset & dim._time_count;
    }

    // explicit instantiation for the correct archive types
    template HPX_COMPONENT_EXPORT void
    serialize(input_archive&, components::TileDimension&, unsigned int const);
    template HPX_COMPONENT_EXPORT void
    serialize(output_archive&, components::TileDimension&, unsigned int const);
}