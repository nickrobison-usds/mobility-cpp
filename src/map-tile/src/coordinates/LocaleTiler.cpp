//
// Created by Nicholas Robison on 7/27/20.
//

#include "map-tile/coordinates/LocaleTiler.hpp"
#include "map-tile/coordinates/Coordinate2D.hpp"

namespace mt::coordinates {
    template<>
    std::vector<LocaleLocator<Coordinate2D>::value>
    LocaleTiler::tile(const Coordinate2D &min, const Coordinate2D &max, const std::array<std::size_t, Coordinate2D::dimensions> &stride) {

        const std::size_t stride_x = stride[0];
        const std::size_t stride_y = stride[1];

        std::vector<LocaleLocator<Coordinate2D>::value> results;
        Coordinate2D current_min = min;
        std::size_t loc_count = 0;
        // Iterate through the coordinate space
        for (std::size_t i = min.get_dim0(); i < max.get_dim0(); i += stride_x) {
            const std::size_t min_x = std::min(i * stride_x, max.get_dim0());
            const std::size_t max_x = std::min(current_min.get_dim0() + stride_x, max.get_dim0());
            for (std::size_t j = min.get_dim1(); j < max.get_dim1(); j += stride_y) {
                const std::size_t min_y = std::min(j * stride_y, max.get_dim1());
                const std::size_t max_y = std::min(j * stride_y + stride_y, max.get_dim1());
                const Coordinate2D max_point(max_x, max_y);
                results.emplace_back(LocaleLocator<Coordinate2D>::mt_tile(current_min, max_point), loc_count);
                current_min = Coordinate2D(current_min.get_dim0(), std::min(max_y + 1, max.get_dim1()));

                loc_count++;
            }
            current_min = Coordinate2D(std::min(max_x + 1, max.get_dim0()), min.get_dim1());
        }

        return results;
    }
}


