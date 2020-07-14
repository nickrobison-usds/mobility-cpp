//
// Created by Nicholas Robison on 5/20/20.
//

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/collectives.hpp>
#include <mpi.h>
#include "spdlog/fmt/fmt.h" // Get FMT from spdlog, to avoid conflicts with other libraries.

const char* scatter_basename = "/mobility/locale/scatter/";
const char* gather_basename = "/mobility/locale/gather/";

using namespace std;

namespace server {
    struct hello_component : hpx::components::component_base<hello_component> {

    public:
        string invoke() {
            return fmt::format("Hello from locale: {}", hpx::get_locality_id());
        }

        HPX_DEFINE_COMPONENT_ACTION(hello_component, invoke, invoke_action);
    };
}
HPX_REGISTER_COMPONENT(hpx::components::component<::server::hello_component>, hello_component);
HPX_REGISTER_ACTION(server::hello_component::invoke_action);

struct hello_component : hpx::components::client_base<hello_component, server::hello_component> {

    typedef hpx::components::client_base<hello_component, server::hello_component> base_type;

    explicit hello_component(hpx::future<hpx::id_type> && f)
    : base_type(std::move(f))
    {}

//    explicit hello_component(hpx::naming::id_type && f)
//    : base_type(std::move(f))
//    {}

    explicit hello_component(size_t num_localities): base_type(hpx::new_<server::hello_component>(hpx::find_here())) {
        hpx::register_with_basename(scatter_basename, get_id(), hpx::get_locality_id());
    }

    ~hello_component() {
        fmt::print("Unregistering on locale {}.\n", hpx::get_locality_id());
        const auto f = hpx::unregister_with_basename(scatter_basename, hpx::get_locality_id());
        hpx::wait_all(f);
    }

    hpx::future<string> invoke() {
        return hpx::async<server::hello_component::invoke_action>(get_id());
    }
};

HPX_REGISTER_GATHER(string, hello_gatherer);

int hpx_main(boost::program_options::variables_map &vm) {
    vector<hpx::naming::id_type> localities = hpx::find_all_localities();
    const auto nl = localities.size();

    std::cout << "Running on locales" << std::endl;

    // Initialize the MPI values
    int mpi_size, mpi_rank;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Info info = MPI_INFO_NULL;
    MPI_Comm_size(comm, &mpi_size);
    MPI_Comm_rank(comm, &mpi_rank);

    fmt::print("Initializing MPI rank {} of {}\n", mpi_rank, mpi_size);

    hello_component component(nl);

    hpx::future<string> result = component.invoke();

    if (0 == hpx::get_locality_id()) {
        hpx::future<vector<string>> overall_result = hpx::lcos::gather_here(gather_basename, move(result), nl);
        vector<string> solution = overall_result.get();

        fmt::print("I have {} results\n", solution.size());

        for(const auto &s : solution) {
            fmt::print("Result: {}\n", s);
        }
    } else {
        hpx::lcos::gather_there(gather_basename, move(result)).wait();
    }

    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    std::cout << "Running from main" << std::endl;
    using namespace hpx::program_options;

    options_description desc_commandline;
    std::vector<std::string> const cfg = {
            "hpx.run_hpx_main!=1"
    };
    return hpx::init(desc_commandline, argc, argv, cfg);
}