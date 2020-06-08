//
// Created by Nicholas Robison on 6/8/20.
//
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"
#include "../src/vector_scaler.hpp"
#include <blaze/math/StaticVector.h>

using namespace components::detail;


TEST_CASE("Simple test", "[vector]") {
    blaze::StaticVector<int, 2UL> v(4);
    auto result = blaze::map(v, VectorScaler<int>(4));
    REQUIRE(blaze::sum(result) == 2);

    blaze::StaticMatrix<int, 2UL, 2UL> m(100000);
    auto r2 = blaze::map(m, VectorScaler<int>(100000));
    auto summed = blaze::sum(r2);
    REQUIRE(summed == 4);
}

TEST_CASE("Double test", "[vector]") {
    blaze::StaticVector<double, 2UL> v(2);
    auto result = blaze::map(v, VectorScaler<double>(4.0));
    REQUIRE(blaze::sum(result) == 1);
}

#ifdef MCPP_AVX2
TEST_CASE("AVX 2 Test", "[vector]") {
    REQUIRE(1 == 1 );
}
#endif

#ifdef MCPP_AVX
TEST_CASE("AVX Test", "[vector]") {
    REQUIRE(1 == 1 );
}
#endif

