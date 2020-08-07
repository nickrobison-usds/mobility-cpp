//
// Created by Nicholas Robison on 7/26/20.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <spatial/GEOSProcessor.hpp>
#include <ogr_geometry.h>
#include "catch2/catch.hpp"
#include <array>
#include <algorithm>

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

//TEST_CASE("Test GEOS", "[spatial]") {
//
//    std::vector<OGRPoint> points = {
//        OGRPoint(-77.0674169, 38.9537949), // DC
//        OGRPoint(-122.3550915, 47.6754785), // Seattle
//
//    };
//
//    OGRPoint p(-85.0310425, 41.2158127); // Leo
//    spatial::geos::GEOSProcessor g;
//    const auto results = g.computeDistances(p, points);
//    const std::vector<double> expected = {8.2786506886383506, 37.878911230420691};
//    REQUIRE_THAT(results, Catch::Approx(expected).epsilon(1.e-5));
//}

TEST_CASE("Test Haversine", "[spatial]") {
    std::vector<OGRPoint> points = {
            OGRPoint(-77.0674169, 38.9537949), // DC
            OGRPoint(-122.3550915, 47.6754785), // Seattle

    };

    OGRPoint p(-85.0310425, 41.2158127); // Leo
    std::vector<double> results;

    std::transform(points.begin(), points.end(), std::back_inserter(results), [&p](const auto &p2) {
        return haversine(p.getY(), p.getX(), p2.getY(), p2.getX());
    });

    const std::vector<double> expected = {722342.4699386562, 3017339.7373509742};
    REQUIRE_THAT(results, Catch::Approx(expected).epsilon(1.e-5));

}

#ifdef USE_METAL
#include <spatial/MetalProcessor.hpp>

TEST_CASE("Test Metal", "[spatial]") {
    std::vector<OGRPoint> points = {
            OGRPoint(-77.0674169, 38.9537949), // DC
            OGRPoint(-122.3550915, 47.6754785), // Seattle

    };

    OGRPoint p(-85.0310425, 41.2158127); // Leo
//    std::vector<double> results;

    spatial::metal::MetalProcessor processor;
    const auto results = processor.computeDistances(p, points);

    const std::vector<double> expected = {722354.5625, 3017495.75};
    REQUIRE_THAT(results, Catch::Approx(expected).margin(300));
}

#endif


