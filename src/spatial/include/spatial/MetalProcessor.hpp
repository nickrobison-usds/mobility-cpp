//
//  MetalProcessor.hpp
//  spatial
//
//  Created by Nicholas Robison on 7/29/20.
//

#ifndef MetalProcessor_hpp
#define MetalProcessor_hpp

#include "spatial/SpatialProcessor.hpp"

namespace spatial::metal {

    class MetalProcessor : public SpatialProcessor<MetalProcessor> {

    public:
        MetalProcessor();

        std::vector<double> computeDistances(const OGRPoint &point, std::vector<OGRPoint> &points) const;
    };
}


#endif /* MetalProcessor_hpp */
