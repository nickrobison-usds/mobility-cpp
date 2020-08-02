//
// Created by Nicholas Robison on 7/26/20.
//

#ifndef MOBILITY_CPP_GEOSPROCESSOR_HPP
#define MOBILITY_CPP_GEOSPROCESSOR_HPP

#include "SpatialProcessor.hpp"

namespace spatial::geos {
    class GEOSProcessor : public SpatialProcessor<GEOSProcessor> {

    public:
        std::vector<double> computeDistances(const OGRPoint &point, std::vector<OGRPoint> &points) const;
    };
}


#endif //MOBILITY_CPP_GEOSPROCESSOR_HPP
