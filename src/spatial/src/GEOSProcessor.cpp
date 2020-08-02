//
// Created by Nicholas Robison on 7/26/20.
//

#include <spatial/GEOSProcessor.hpp>
#include <algorithm>

namespace spatial::geos {

    std::vector<double> GEOSProcessor::computeDistances(const OGRPoint &point, std::vector<OGRPoint> &points) const {
        std::vector<double> results;
        results.reserve(points.size());
        std::transform(points.begin(), points.end(), std::back_inserter(results), [&point](const OGRGeometry &p) {
            return point.Distance(&p);
        });

        return results;
    }
}