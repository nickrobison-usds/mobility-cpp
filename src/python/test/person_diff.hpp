//
// Created by Nicholas Robison on 10/1/20.
//

#include <boost/hana.hpp>
#include <string>

struct PersonDiff {
    std::string name;
    int age;
    float age_diff;
};

BOOST_HANA_ADAPT_STRUCT(PersonDiff, name, age, age_diff);

