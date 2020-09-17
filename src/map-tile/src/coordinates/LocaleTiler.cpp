//
// Created by Nicholas Robison on 7/27/20.
//

#include "map-tile/coordinates/LocaleTiler.hpp"
#include "map-tile/coordinates/Coordinate2D.hpp"
#include "map-tile/coordinates/Coordinate3D.hpp"

namespace mt::coordinates {
    template<>
    std::vector<LocaleLocator<Coordinate2D>::value>
    LocaleTiler::tile(const Coordinate2D &min, const Coordinate2D &max,
                      const std::array<std::size_t, Coordinate2D::dimensions> &stride) {

        const std::size_t stride_x = stride[0];
        const std::size_t stride_y = stride[1];

        if (stride_x == 0 || stride_y == 0) {
            throw std::invalid_argument("Cannot tile with stride of 0");
        }

        std::vector<LocaleLocator<Coordinate2D>::value> results;
        std::size_t loc_count = 0;
        // Iterate through the coordinate space
        for (std::size_t dim0 = min.get_dim0(); dim0 < max.get_dim0(); dim0 += stride_x) {
            for (std::size_t dim1 = min.get_dim1(); dim1 < max.get_dim1(); dim1 += stride_y) {
                const Coordinate2D min_point(dim0, dim1);
                const Coordinate2D max_point(std::min(dim0 + stride_x, max.get_dim0()),
                                             std::min(dim1 + stride_y, max.get_dim1()));
                results.emplace_back(LocaleLocator<Coordinate2D>::mt_tile(min_point, max_point), loc_count);
                loc_count++;
            }
        }

        return results;
    }

    template<>
    std::vector<LocaleLocator<Coordinate3D>::value>
    LocaleTiler::tile(const Coordinate3D &min, const Coordinate3D &max,
                      const std::array<std::size_t, Coordinate3D::dimensions> &stride) {

        const std::size_t stride_x = stride[0];
        const std::size_t stride_y = stride[1];
        const std::size_t stride_z = stride[2];

        if (stride_x == 0 || stride_y == 0 || stride_z == 0) {
            throw std::invalid_argument("Cannot tile with stride of 0");
        }

        std::vector<LocaleLocator<Coordinate3D>::value> results;
        Coordinate3D current_min = min;
        std::size_t loc_count = 0;
        // Iterate through the coordinate space

        for (std::size_t dim0 = min.get_dim0(); dim0 < max.get_dim0(); dim0 += stride_x) {
            for (std::size_t dim1 = min.get_dim1(); dim1 < max.get_dim1(); dim1 += stride_y) {
                for (std::size_t dim2 = min.get_dim2(); dim2 < max.get_dim2(); dim2 += stride_z) {
                    const Coordinate3D min_point(dim0, dim1, dim2);
                    const Coordinate3D max_point(std::min(dim0 + stride_x, max.get_dim0()),
                                                 std::min(dim1 + stride_y, max.get_dim1()),
                                                 std::min(dim2 + stride_z, max.get_dim2()));
                    results.emplace_back(LocaleLocator<Coordinate3D>::mt_tile(min_point, max_point), loc_count);
                    loc_count++;
                }
            }
        }

        return results;
    }
}


