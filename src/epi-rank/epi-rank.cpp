//
// Created by Nicholas Robison on 9/11/20.
//

#include "SafegraphCountyMapper.hpp"
#include "SafegraphCountyTiler.hpp"
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include <map-tile/MapTileBuilder.hpp>
#include <map-tile/client/MapTileClient.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/io/FileProvider.hpp>
#include <shared/data.hpp>

// Register the map-tile instance
typedef vector<cbg_centrality> reduce_type;
REGISTER_MAPPER(county_visit, mt::coordinates::Coordinate3D, SafegraphCountyMapper, SafegraphCountyTiler, reduce_type, string,
        mt::io::FileProvider);

int hpx_main(hpx::program_options::variables_map &vm) {
    return hpx::finalize();
};

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("config", value<std::string>()->default_value("./config.yml"), "Config file location");

    return hpx::init(desc_commandline, argc, argv);
}
