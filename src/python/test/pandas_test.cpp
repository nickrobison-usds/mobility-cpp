//
// Created by Nicholas Robison on 9/10/20.
//

#include "person.hpp"
#include "person_diff.hpp"
#include <python/PandasEngine.hpp>
#include "catch2/catch.hpp"

TEST_CASE("Simple Pandas Test", "[pandas]") {
    mcpp::python::PandasEngine<Person> p("data.simple_test", 3);
    p.load({"Nick Robison", 31});
    p.load({"Tom Ford", 20});
    p.load({"Bruce Willis", 42});
    p.evaluate();
}

TEST_CASE("Missing python file", "[pandas]") {
    mcpp::python::PandasEngine<Person> p("data.missing", 1);
    p.load({"Nick Robison", 31});
    REQUIRE_THROWS_WITH(p.evaluate(), "ModuleNotFoundError: No module named 'data.missing'");
}

TEST_CASE("Missing python directory", "[pandas]") {
    mcpp::python::PandasEngine<Person> p("datanope.missing", 1);
    p.load({"Nick Robison", 31});
    REQUIRE_THROWS_WITH(p.evaluate(), "ModuleNotFoundError: No module named 'datanope'");
}

TEST_CASE("Missing compute function", "[python]") {
    mcpp::python::PandasEngine<Person> p("data.missing_compute", 1);
    p.load({"Nick Robison", 31});
    REQUIRE_THROWS_WITH(p.evaluate(), "AttributeError: module 'data.missing_compute' has no attribute 'compute'");
}

TEST_CASE("Test return type", "[python]") {
    mcpp::python::PandasEngine<Person, PersonDiff> p("data.return_test", 3);
    p.load({"Nick Robison", 31});
    p.load({"Tom Ford", 20});
    p.load({"Bruce Willis", 42});
    const auto r = p.evaluate();
    REQUIRE(r.size() == 3);
    REQUIRE(r.at(0).age_diff == 0.0f);
}

