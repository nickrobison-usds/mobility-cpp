//
// Created by Nicholas Robison on 7/19/20.
//


#define CATCH_CONFIG_RUNNER

#include <hpx/config.hpp>
#include <hpx/hpx_main.hpp>
#include <absl/strings/str_split.h>
#include "catch2/catch.hpp"
#include "map-tile/Context.hpp"
#include "map-tile/MapTileClient.hpp"
#include "map-tile/server/LocaleLocator.hpp"

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

    friend class hpx::serialization::access;
    template<typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & airline & airline_id
        & src_airport & src_airport_id
        & dest_airport & dest_airport_id
        & code_share & stops & equipment;
    }
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

//        ctx.emit(f);
    }
};

REGISTER_MAPPER(FlightInfo, FlightInfo, FlightMapper, std::string, mt::io::FileProvider);

TEST_CASE("Compiles", "[map-tile]") {
    const mt::coordinates::LocaleLocator l{};
    std::vector<string> files{"data/routes.csv"};
    mt::MapTileClient<FlightInfo, FlightInfo, FlightMapper> server(l, files);
    server.tile();
    REQUIRE(1 == 1);
}

