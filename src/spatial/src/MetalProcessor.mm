//
//  MetalProcessor.cpp
//  spatial
//
//  Created by Nicholas Robison on 7/29/20.
//

#include "../include/spatial/MetalProcessor.hpp"
#include <simd/simd.h>
#include <array>
#import <Metal/Metal.h>

namespace spatial::metal {

MetalProcessor::MetalProcessor() {
    // Initialize the things
}

std::vector<double> MetalProcessor::computeDistances(const OGRPoint &point, std::vector<OGRPoint> &points) const {
    const auto device = MTLCreateSystemDefaultDevice();
    
    const auto bundle = [NSBundle bundleWithIdentifier:@"com.nickrobison.spatial"];

    const auto rsc = [bundle pathForResource:@"spatial" ofType:@"metallib"];

    NSError *nerr;

    const id <MTLLibrary> library = [device newLibraryWithFile:rsc error:&nerr];
    const id <MTLFunction> haversineFunction = [library newFunctionWithName:@"haversine"];
    
    // Build the pipeline

    const auto pipeline = [device newComputePipelineStateWithFunction:haversineFunction error:&nerr];
    
    // Copy into a single data buffer
    std::vector<simd::float2> point_data;
    for (const auto &p : points) {
        simd::float2 f;
        f.x = p.getX();
        f.y = p.getY();
        point_data.push_back(f);
//        point_data.push_back(static_cast<float &&>(p.getX()));
//        point_data.push_back(static_cast<float &&>(p.getY()));
    }
    
    // Figure out how to encode the parameters into a buffer
//    HaversineParameters params;
//    params.point.x = point.getX();
//    params.point.y = point.getY();
//
//    const auto arg_encoder = [haversineFunction newArgumentEncoderWithBufferIndex:0];
//
//    const auto argumentBuffer = [device newBufferWithBytes:<#(nonnull const void *)#> length:<#(NSUInteger)#> options:<#(MTLResourceOptions)#>]b
    
    std::array<simd::float2, 1> single_point{simd::float2{static_cast<float>(point.getX()), static_cast<float>(point.getY())}};
    
    const auto pointBuffer = [device newBufferWithBytes:single_point.data() length:single_point.size() * sizeof(simd::float2) options:MTLResourceStorageModeShared];
    
    const auto pointsBuffer = [device newBufferWithBytes:point_data.data() length:point_data.size() * sizeof(simd::float2) options:MTLResourceStorageModeShared];
    
    const auto outBuffer = [device newBufferWithLength:points.size() * sizeof(float) options:MTLResourceStorageModeShared];
    
    
    const auto queue = [device newCommandQueue];
    const auto buffer = [queue commandBuffer];
    const auto encoder = [buffer computeCommandEncoder];
    [encoder setBuffer:pointBuffer offset:0 atIndex:0];
    [encoder setBuffer:pointsBuffer offset:0 atIndex:1];
    [encoder setBuffer:outBuffer offset:0 atIndex:2];
    [encoder setComputePipelineState:pipeline];
    
    const auto grid_size = MTLSizeMake(pointsBuffer.length, 1, 1);
    const auto threads = pipeline.maxTotalThreadsPerThreadgroup > pointsBuffer.length ? pointsBuffer.length : pipeline.maxTotalThreadsPerThreadgroup;
    const auto group_size = MTLSizeMake(threads, 1, 1);
    
    [encoder dispatchThreadgroups:grid_size threadsPerThreadgroup:group_size];
    
    [encoder endEncoding];
    [buffer commit];
    [buffer waitUntilCompleted];
    
    std::vector<double> results;
    results.assign((float*) outBuffer.contents, (float *) outBuffer.contents + outBuffer.length / sizeof(float));
    
    return results;
}
}
