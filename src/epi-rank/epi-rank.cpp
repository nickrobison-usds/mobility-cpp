//
// Created by Nicholas Robison on 9/11/20.
//

#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>

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
