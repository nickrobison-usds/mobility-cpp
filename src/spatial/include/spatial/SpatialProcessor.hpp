//
// Created by Nicholas Robison on 7/26/20.
//

#ifndef MOBILITY_CPP_SPATIALPROCESSOR_HPP
#define MOBILITY_CPP_SPATIALPROCESSOR_HPP

#include <vector>
#include <ogr_geometry.h>

namespace spatial {

    template<typename Processor>
    class SpatialProcessor {
    public:
        std::vector<double> computeDistances(const OGRPoint &point, std::vector<OGRPoint> &points) const {
            return static_cast<Processor const&> (*this).computeDistances(point, points);
        }

    };
}

#endif //MOBILITY_CPP_SPATIALPROCESSOR_HPP
