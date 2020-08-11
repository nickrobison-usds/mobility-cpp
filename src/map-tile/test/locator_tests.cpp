//
// Created by Nicholas Robison on 7/23/20.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"
#include "map-tile/coordinates/LocaleLocator.hpp"
#include "map-tile/coordinates/Coordinate2D.hpp"
#include "map-tile/coordinates/Coordinate3D.hpp"

using namespace std;
using namespace mt::coordinates;

TEST_CASE("Simple Locator Test", "[locator]") {

    // Let's create two tiles that cover a 5x5 grid
    const auto c1 = Coordinate2D(0, 0);
    const auto c2 = Coordinate2D(3, 5);

    const auto d1 = Coordinate2D(3, 0);
    const auto d2 = Coordinate2D(5, 5);

    const auto l = LocaleLocator<Coordinate2D>({
                                                       LocaleLocator<Coordinate2D>::value{
                                                               LocaleLocator<Coordinate2D>::mt_tile(c1, c2), 1},
                                                       LocaleLocator<Coordinate2D>::value{
                                                               LocaleLocator<Coordinate2D>::mt_tile(d1, d2), 2},
                                               });

    // Try in the center of one
    const auto loc1 = l.get_locale(Coordinate2D(1, 1));
    REQUIRE(loc1 == 1);

    const auto loc2 = l.get_locale(Coordinate2D(4, 4));
    REQUIRE(loc2 == 2);

    // Now, right on the edge
    const auto loc3 = l.get_locale(Coordinate2D(3, 1));
    REQUIRE(loc3 == 2);

    // Now, right on the edge
    const auto loc4 = l.get_locale(Coordinate2D(3, 3));
    REQUIRE(loc4 == 2);

    const auto loc5 = l.get_locale(Coordinate2D(0, 3));
    REQUIRE(loc5 == 1);

    REQUIRE_THROWS_WITH(l.get_locale(Coordinate2D(7, 7)), "Out of bounds");
    REQUIRE_THROWS_WITH(l.get_locale(Coordinate2D(3, 5)), "Out of bounds");
    REQUIRE_THROWS_WITH(l.get_locale(Coordinate2D(5, 5)), "Out of bounds");
}

TEST_CASE("3D Locator Test", "[locator]") {

    // Let's create two tiles that cover a 5x5x2 grid
    const auto c1 = Coordinate3D(0, 0, 0);
    const auto c2 = Coordinate3D(2, 5, 2);

    const auto d1 = Coordinate3D(3, 0, 0);
    const auto d2 = Coordinate3D(5, 5, 2);

    const auto l = LocaleLocator<Coordinate3D>({
                                                       LocaleLocator<Coordinate3D>::value{
                                                               LocaleLocator<Coordinate3D>::mt_tile(c1, c2), 1},
                                                       LocaleLocator<Coordinate3D>::value{
                                                               LocaleLocator<Coordinate3D>::mt_tile(d1, d2), 2},
                                               });

    // Try in the center of one
    const auto loc1 = l.get_locale(Coordinate3D(1, 1, 1));
    REQUIRE(loc1 == 1);

    const auto loc2 = l.get_locale(Coordinate3D(4, 4, 1));
    REQUIRE(loc2 == 2);

    // Now, right on the edge
    const auto loc3 = l.get_locale(Coordinate3D(3, 0, 0));
    REQUIRE(loc3 == 2);

    // Now, right on the edge
    const auto loc4 = l.get_locale(Coordinate3D(0, 1, 0));
    REQUIRE(loc4 == 1);

    REQUIRE_THROWS_WITH(l.get_locale(Coordinate3D(7, 7, 7)), "Out of bounds");
    REQUIRE_THROWS_WITH(l.get_locale(Coordinate3D(5, 5, 2)), "Out of bounds");
}