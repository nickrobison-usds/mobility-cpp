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
    io::CSVLoader<6, false> loader("data/addresses.csv");

    string fn;
    string ln;
    string add;
    string ct;
    string st;
    int zp;

    const auto v = loader.read<address>([](const string& first_name, const string& last_name, const string& address, const string& city, const string& state, const int zip) {
        struct address a = {first_name, last_name, address, city, state, zip};
        return a;
    }, fn, ln, add, ct, st, zp);

    REQUIRE(v.size() == 5);
    REQUIRE(v.at(0).first_name == "John");
    REQUIRE(v.at(4).first_name.empty());

}

TEST_CASE("Skips CSV header", "[csv]") {
    io::CSVLoader<6, true> loader("data/addresses_header.csv");

    string fn;
    string ln;
    string add;
    string ct;
    string st;
    int zp;

    const auto v = loader.read<address>([](const string& first_name, const string& last_name, const string& address, const string& city, const string& state, const int zip) {
        struct address a = {first_name, last_name, address, city, state, zip};
        return a;
    }, fn, ln, add, ct, st, zp);

    REQUIRE(v.size() == 5);
    REQUIRE(v.at(0).first_name == "John");
    REQUIRE(v.at(4).first_name.empty());

}
