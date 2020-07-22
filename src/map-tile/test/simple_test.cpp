//
// Created by Nicholas Robison on 7/19/20.
//


#define CATCH_CONFIG_RUNNER

#include <hpx/hpx_main.hpp>
#include <absl/strings/str_split.h>
#include "catch2/catch.hpp"
#include "map-tile/Context.hpp"
#include "map-tile/MapTileClient.hpp"

int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}

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

struct FlightMapper {
    void map(const mt::MapContext<FlightInfo> &ctx, const std::string &info) const {
        const std::vector<std::string> splits = absl::StrSplit(info, ',');
        splits.size();
        const FlightInfo f{
            splits[0],
            splits[1],
            splits[2],
            splits[3],
            splits[4],
            splits[5],
            !splits[6].empty(),
            0,
            0
        };

        ctx.emit(f);
    }
};

REGISTER_MAPPER(FlightInfo, FlightInfo, FlightMapper, std::string, mt::io::FileProvider);

TEST_CASE("Compiles", "[map-tile]") {
    std::vector<string> files{"data/routes.csv"};
    mt::MapTileClient<FlightInfo, FlightInfo, FlightMapper> server(files);
    server.tile();
    REQUIRE(1 == 1);
}

