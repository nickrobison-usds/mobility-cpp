//
// Created by Nicholas Robison on 9/10/20.
//

#include <boost/hana.hpp>
#include <python/PandasEngine.hpp>
#include "catch2/catch.hpp"

struct Person {
    int name;
    int age;
};

BOOST_HANA_ADAPT_STRUCT(Person, name, age);

TEST_CASE("Simple Pandas Test", "[python]") {
    mcpp::python::PandasEngine<Person> p(1);
    p.load({40, 31});
    REQUIRE(p.evaluate() == "1");
}