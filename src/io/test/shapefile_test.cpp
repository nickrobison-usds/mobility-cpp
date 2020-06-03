//
// Created by Nicholas Robison on 5/22/20.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"
#include "io/ShapefileWrapper.hpp"
#include "ogrsf_frmts.h"

using namespace std;

struct poly {
    const double_t area; // AREA
    const int64_t id; // EAS_ID
    const string description; // PRFEDEA
};

TEST_CASE("Loads Shapefile", "[shapefile]") {
    io::Shapefile s("data/poly.shp");
    const auto v = s.read<poly>([](const OGRFeatureUniquePtr &s) {
        poly p = {
            s->GetFieldAsDouble("AREA"),
            s->GetFieldAsInteger64("EAS_ID"),
            s->GetFieldAsString("PRFEDEA")
        };
        return p;
    });
    REQUIRE(v.size() == 10);
    REQUIRE(v.at(0).description == "35043411");
}
