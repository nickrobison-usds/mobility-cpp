//
// Created by Nicholas Robison on 9/9/20.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <python/PythonInterpreter.hpp>
#include "catch2/catch.hpp"

TEST_CASE("Simple Python Test", "[python]") {
    mcpp::python::PythonInterpreter p;

    REQUIRE(p.evaluate<int>("2*2") == 4);
}

TEST_CASE("Failing Python Test", "[python]") {
    mcpp::python::PythonInterpreter p;

    REQUIRE_THROWS_WITH(p.evaluate<int>("48.split(',')"), "SyntaxError: ('invalid syntax', ('<string>', 2, 4, \"48.split(',')\"))");
}

