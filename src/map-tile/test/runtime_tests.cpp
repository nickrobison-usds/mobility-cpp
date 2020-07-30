//
// Created by Nicholas Robison on 7/19/20.
//


#define CATCH_CONFIG_RUNNER

#include <hpx/config.hpp>
#include <hpx/hpx_main.hpp>
#include <absl/strings/str_split.h>
#include <boost/bimap.hpp>
#include "catch2/catch.hpp"
#include <io/csv_reader.hpp>
#include "map-tile/ctx/Context.hpp"
#include "map-tile/client/MapTileClient.hpp"
#include "map-tile/coordinates/LocaleLocator.hpp"
#include "map-tile/coordinates/Coordinate2D.hpp"
#include <Eigen/Sparse>
#include <atomic>
#include <iostream>
#include <map-tile/coordinates/LocaleTiler.hpp>

int main(int argc, char *argv[]) {
    return Catch::Session().run(argc, argv);
}

// Some atomics
std::atomic_int flights(0);

typedef boost::bimap<std::string, std::size_t> flight_bimap;
typedef flight_bimap::value_type position;


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

    void setup(const mt::ctx::MapContext<FlightInfo, mt::coordinates::Coordinate2D> &ctx) {
        io::CSVLoader<2, true> loader("data/airports.csv");

        int id;
        std::string airport;

        const auto v  = loader.read<position>([](const int &id, const std::string &apt) {
            return position(apt, id);
        }, id, airport);

        std::for_each(v.begin(), v.end(), [this](const auto &pair) {
            _airports.insert(pair);
        });
    }

    void map(const mt::ctx::MapContext<FlightInfo, mt::coordinates::Coordinate2D> &ctx, const std::string &info) const {
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

        // Lookup the airport ID
        const auto src_iter = _airports.left.find(f.src_airport);
        if (src_iter == _airports.left.end()) {
            throw new std::invalid_argument("Cannot find airport ID");
        }
        const auto dest_iter = _airports.left.find(f.dest_airport);
        if (dest_iter == _airports.left.end()) {
            throw new std::invalid_argument("Cannot find airport ID");
        }

        const mt::coordinates::Coordinate2D coord(src_iter->second, dest_iter->second);

        ctx.emit(coord, f);
    }

private:
    flight_bimap _airports;
};

struct FlightTile {

    FlightTile(): _flight_matrix(3425, 3425) {
        // Not used
    }

    void receive(const mt::ctx::ReduceContext<FlightInfo, mt::coordinates::Coordinate2D> &ctx, const mt::coordinates::Coordinate2D &key,
                 const FlightInfo &value) {
        // Just increment a simple counter
        std::cout << "Receiving" << std::endl;
        _flight_matrix.coeffRef(key.get_dim0(), key.get_dim1()) += 1;
    }

    void compute() {
        flights += _flight_matrix.sum();
    }

private:
    Eigen::SparseMatrix<unsigned int> _flight_matrix;
};

REGISTER_MAPPER(FlightInfo, mt::coordinates::Coordinate2D, FlightMapper, FlightTile, std::string, mt::io::FileProvider);

TEST_CASE("Flight Mapper", "[integration]") {
    using namespace mt::coordinates;
    const auto locales = hpx::get_num_localities().get();
    std::cout << "Locales: " << locales << std::endl;
    const auto c1 = Coordinate2D(0, 0);
    const auto c2 = Coordinate2D(3425, 3425);

    // Divide up the stride based on the number of locales that we have
    const std::size_t x_stride = floor(3425 / locales);

    const auto tiles = LocaleTiler::tile(c1, c2, {
        x_stride,
        3425
    });
    const mt::coordinates::LocaleLocator<Coordinate2D> l(tiles);
    std::vector<string> files{"data/routes.csv"};

    // Run a client on each locale
    std::vector<hpx::future<void>> results;
    for (const auto &loc : hpx::find_all_localities()) {
        mt::client::MapTileClient<FlightInfo, Coordinate2D, FlightMapper, FlightTile> server(loc, l, files);
        results.push_back(std::move(server.tile()));
    }
    hpx::wait_all(results);

    REQUIRE(flights == (67663 * locales));
}

