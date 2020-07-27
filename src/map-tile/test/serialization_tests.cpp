//
// Created by Nicholas Robison on 7/24/20.
//

#include <map-tile/coordinates/Coordinate3D.hpp>
#include "catch2/catch.hpp"
#include "map-tile/coordinates/Coordinate2D.hpp"
#include "map-tile/coordinates/LocaleLocator.hpp"

TEST_CASE("Coordinate2D serialization", "[serialization]") {
    const mt::coordinates::Coordinate2D coord(1, 2);
    std::ofstream ofs("coord-out");

    std::vector<char> out_buffer;

    hpx::serialization::output_archive oa(out_buffer);
    oa << coord;

    hpx::serialization::input_archive ia(out_buffer, out_buffer.size());
    mt::coordinates::Coordinate2D c2;
    ia >> c2;
    REQUIRE(coord == c2);
}

TEST_CASE("Coordinate3D serialization", "[serialization]") {
    const mt::coordinates::Coordinate3D coord(1, 2, 4);
    std::ofstream ofs("coord-out");

    std::vector<char> out_buffer;

    hpx::serialization::output_archive oa(out_buffer);
    oa << coord;

    hpx::serialization::input_archive ia(out_buffer, out_buffer.size());
    mt::coordinates::Coordinate3D c2;
    ia >> c2;
    REQUIRE(coord == c2);
}

TEST_CASE("LocaleLocator serialization", "[serialization]") {
    using namespace mt::coordinates;

    const auto c1 = Coordinate2D(0, 0);
    const auto c2 = Coordinate2D(2, 5);

    const auto d1 = Coordinate2D(0, 3);
    const auto d2 = Coordinate2D(5, 5);

    const auto l = LocaleLocator<Coordinate2D>({
                                         LocaleLocator<Coordinate2D>::value{LocaleLocator<Coordinate2D>::mt_tile(c1, c2), 1},
                                         LocaleLocator<Coordinate2D>::value{LocaleLocator<Coordinate2D>::mt_tile(d1, d2), 2},
                                 });

    std::vector<char> out_buffer;
    hpx::serialization::output_archive oa(out_buffer);
    oa << l;

    LocaleLocator<Coordinate2D> l2;
    hpx::serialization::input_archive ia(out_buffer, out_buffer.size());
    ia >> l2;

    const auto loc1 = l2.get_locale(Coordinate2D(3, 5));
    REQUIRE(loc1 == 2);

}