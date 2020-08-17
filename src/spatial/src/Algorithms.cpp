//
//  Algorithms.cpp
//  spatial
//
//  Created by Nicholas Robison on 7/26/20.
//

#include <cmath>

namespace spatial::algorithms {

const static double R = 6371e3;

double haversine(const double &lat1, const double &lon1, const double &lat2, const double &lon2) {
    // distance between latitudes
          // and longitudes
          double dLat = (lat2 - lat1) *
    M_PI / 180.0;
    double dLon = (lon2 - lon1) *
    M_PI / 180.0;
    
    // convert to radians
    const auto lat1_rad = (lat1) * M_PI / 180.0;
    const auto lat2_rad = (lat2) * M_PI / 180.0;
    
    // apply formulae
    double a = pow(sin(dLat / 2), 2) +
    pow(sin(dLon / 2), 2) *
    cos(lat1_rad) * cos(lat2_rad);
    double c = 2 * asin(sqrt(a));
    return R * c;
}
}
