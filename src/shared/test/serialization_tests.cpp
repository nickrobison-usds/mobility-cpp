//
// Created by Nicholas Robison on 8/6/20.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"
#include <shared/data.hpp>
#include <date/date.h>
#include <hpx/serialization/output_archive.hpp>

TEST_CASE("V2 serialization", "[serialization]") {
    const v2 orig{"hello", date::sys_days{} + date::days{1000}, "loc", "cbg", 1, 1.5, 3.1};

    std::vector<char> out_buffer;
    hpx::serialization::output_archive oa(out_buffer);
    oa << orig;

    hpx::serialization::input_archive ia(out_buffer, out_buffer.size());
    v2 deser;
    ia >> deser;
    REQUIRE(deser == orig);
}

TEST_CASE("County visit serialization", "[serialization]") {
    const v2 orig{"hello", date::sys_days{} + date::days{1000}, "loc", "cbg", 1};

    std::vector<char> out_buffer;
    hpx::serialization::output_archive oa(out_buffer);
    oa << orig;

    hpx::serialization::input_archive ia(out_buffer, out_buffer.size());
    v2 deser;
    ia >> deser;
    REQUIRE(deser == orig);
}
