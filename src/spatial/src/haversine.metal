//
//  haversine.metal
//  mobility-cpp
//
//  Created by Nicholas Robison on 7/29/20.
//

#include <metal_stdlib>
using namespace metal;

constant float R = 6371e3;
constant float DEGREE_RAD = M_PI_F / 180.0;

struct HaversineParameters {
    float2 point;
};


kernel void haversine(const device float2 *params [[ buffer(0) ]],
                      const device float2 *points [[ buffer(1) ]],
                      device float *out [[ buffer(2) ]],
                      uint id [[ thread_position_in_grid ]]) {
    const auto to_convert = points[id];
    const auto root_point = params[0];
    const float dLat = (to_convert.y - root_point.y) * DEGREE_RAD;
    
    const float dLon = (to_convert.x - root_point.x) * DEGREE_RAD;
    
    const auto lat1_rad = (root_point.y) * DEGREE_RAD;
    const auto lat2_rad = (to_convert.y) * DEGREE_RAD;
    
    const float a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1_rad) * cos(lat2_rad);
    
    const float c = 2 * asin(sqrt(a));
    const auto o = R * c;
    out[id] = o;
}
