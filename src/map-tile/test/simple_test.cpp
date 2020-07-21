//
// Created by Nicholas Robison on 7/19/20.
//


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <absl/strings/str_split.h>
#include "catch2/catch.hpp"
#include "map-tile/Context.hpp"
#include "map-tile/MapTileServer.hpp"

struct FlightInfo {
    std::string airline;
    std::string airline_id;
    std::string src_airport;
    std::string src_airport_id;
    std::string dest_airport;
    std::string dest_airport_id;
    bool code_share;
    int stops;
    int equipment;
};

template<typename, typename>
struct FlightMapper {
    void map(const mt::MapContext<FlightInfo> &ctx, const std::string &info) {
        const std::vector<std::string> splits = absl::StrSplit(info, ',');
        splits.size();
        const FlightInfo f{
            splits[0],
            splits[1],
            splits[2],
            splits[3],
            splits[4],
            splits[5],
            splits[6] != "",
            0,
            0
        };

        ctx.emit(f);
    }
};

TEST_CASE("Compiles", "[map-tile]") {
    std::vector<string> files{"data/routes.csv"};
    mt::server::MapTileServer<FlightInfo, FlightInfo, FlightMapper> server(files);
    server.tile();
    REQUIRE(1 == 1);
}

