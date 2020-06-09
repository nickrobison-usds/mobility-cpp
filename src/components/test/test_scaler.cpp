//
// Created by Nicholas Robison on 6/8/20.
//
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"
#include "../src/vector_scaler.hpp"
#include <blaze/math/StaticVector.h>

using namespace components::detail;


TEST_CASE("Integral test test", "[vector]") {
    blaze::StaticVector<int, 8UL> v(4);
    auto result = blaze::map(v, VectorScaler<int>(4));
    REQUIRE(blaze::sum(result) == 8);

    blaze::StaticMatrix<int, 80UL, 80UL> m(100000);
    auto r2 = blaze::map(m, VectorScaler<int>(100000));
    REQUIRE(blaze::sum(r2) == 6400);
}

TEST_CASE("Double test", "[vector]") {
    blaze::StaticVector<double, 8UL> v(2);
    const auto r = blaze::map(v, VectorScaler<double>(3.0));
    REQUIRE(blaze::sum(r) == Approx(5.33).epsilon(0.33));
}

TEST_CASE("Unaligned test", "[vector]") {
    blaze::StaticVector<int, 10UL> v(4);
    auto result = blaze::map(v, VectorScaler<int>(4));
    REQUIRE(blaze::sum(result) == 10);

    blaze::StaticMatrix<int, 500UL, 500UL> m(100000);
    auto r2 = blaze::map(m, VectorScaler<int>(100000));
    REQUIRE( blaze::sum(r2) == 250000);

    blaze::StaticVector<double, 17UL> dv(2);
    auto r3 = blaze::map(dv, VectorScaler<double>(3.0));
    REQUIRE(blaze::sum(r3) == Approx(11.33).epsilon(0.33));
}

