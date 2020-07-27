//
// Created by Nicholas Robison on 7/27/20.
//

#include <map-tile/coordinates/LocaleTiler.hpp>
#include <map-tile/coordinates/Coordinate2D.hpp>
#include "catch2/catch.hpp"

using namespace mt::coordinates;

TEST_CASE("Simple Tiler Tests", "[tiler]") {

    // Try to tile just a couple of square tiles
    const auto c1 = Coordinate2D(0, 0);
    const auto c2 = Coordinate2D(6, 6);

    const std::array<std::size_t, 2> stride{static_cast<std::size_t>(floor(6 / 2)),
                                            static_cast<std::size_t>(floor(6 / 2))};
    const auto results = LocaleTiler::tile(c1, c2, stride);
    REQUIRE(results.size() == 4);
    REQUIRE(results[2].first.min_corner() == Coordinate2D(4, 0));
    REQUIRE(results[2].first.max_corner() == Coordinate2D(6, 3));
}

TEST_CASE("Uneven Tiler Tests", "[tiler]") {
    // Try to tile just a couple of square tiles
    const auto c1 = Coordinate2D(0, 0);
    const auto c2 = Coordinate2D(5, 5);

    const std::array<std::size_t, 2> stride{static_cast<std::size_t>(2),
                                            static_cast<std::size_t>(2)};
    const auto results = LocaleTiler::tile(c1, c2, stride);
    REQUIRE(results.size() == 9);
    REQUIRE(results[2].first.min_corner() == Coordinate2D(0, 5));
    REQUIRE(results[2].first.max_corner() == Coordinate2D(2, 5));

    // Final tile should be a single value
    REQUIRE(results[8].first.min_corner() == Coordinate2D(5, 5));
    REQUIRE(results[8].first.max_corner() == Coordinate2D(5, 5));
}