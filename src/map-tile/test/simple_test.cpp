//
// Created by Nicholas Robison on 7/19/20.
//


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"
#include "map-tile/MapTileServer.hpp"
#include "../../io/include/io/csv_reader.hpp"

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

template<typename>
struct FlightProvider {

    explicit FlightProvider(const std::string &file): _loader(file) {
        // Not used
    }

    std::vector<FlightInfo> provide() {
        std::string airline;
        std::string airline_id;
        std::string src_airport;
        std::string src_airport_id;
        std::string dest_airport;
        std::string dest_airport_id;
        std::string code_share;
        int stops;
        int equipment;

        return _loader.read<FlightInfo>([](const std::string &airline, const std::string &airline_id, const std::string &src_airport, const std::string ) {
            struct FlightInfo f;

            return f;
        });
    }



private:
    io::CSVLoader<9, false> _loader;
};

template<typename, typename>
struct FlightMapper {

    void map(const std::string &info) {
        // Not used
    }
};

TEST_CASE("Compiles", "[map-tile]") {
    std::vector<string> files{"data/routes.csv"};
    mt::server::MapTileServer<FlightInfo, FlightInfo, FlightMapper> server(files);
    server.tile();
    REQUIRE(1 == 1);
}

