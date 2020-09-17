//
// Created by Nicholas Robison on 9/17/20.
//

#include "person.hpp"
#include <catch2/catch.hpp>
#include <map-tile/coordinates/Coordinate2D.hpp>
#include <python/PythonTiler.hpp>

TEST_CASE("Simpler Tiler Test", "[python]") {
    mcpp::python::PythonTiler<mt::coordinates::Coordinate2D, Person> t;
//    t.setup();
}
