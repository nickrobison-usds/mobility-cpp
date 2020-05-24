//
// Created by Nicholas Robison on 5/23/20.
//
#include "catch2/catch.hpp"
#include "io/csv_reader.hpp"

using namespace std;

struct address {
    const string first_name;
    const string last_name;
    const string address;
    const string city;
    const string state;
    const int32_t zip;
};

TEST_CASE("Loads CSV", "[csv]") {
    io::CSVLoader<2> loader("data/addresses.csv");

    string fn;
    string ln;

    const auto v = loader.read<address>([](const string& first_name, const string& last_name) {
        return address{};
    }, fn, ln);
}