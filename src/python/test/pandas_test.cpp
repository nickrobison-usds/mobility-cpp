//
// Created by Nicholas Robison on 9/10/20.
//

#include <boost/hana.hpp>
#include <python/PandasEngine.hpp>
#include "catch2/catch.hpp"

struct Person {
    std::string name;
    int age;
};

BOOST_HANA_ADAPT_STRUCT(Person, name, age);

TEST_CASE("Simple Pandas Test", "[python]") {
    mcpp::python::PandasEngine<Person> p("simple_test", 1);
    p.load({"Nick Robison", 31});
    p.load({"Tom Ford", 20});
    p.load({"Bruce Willis", 42});
    REQUIRE(p.evaluate() == "12");
}