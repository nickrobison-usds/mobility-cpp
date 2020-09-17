//
// Created by Nicholas Robison on 9/17/20.
//

#include <boost/hana.hpp>
#include <string>

struct Person {
    std::string name;
    int age;
};

BOOST_HANA_ADAPT_STRUCT(Person, name, age);