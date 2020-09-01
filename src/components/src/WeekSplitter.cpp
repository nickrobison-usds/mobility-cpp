//
// Created by Nicholas Robison on 5/19/20.
//
#include "components/WeekSplitter.hpp"

#include "spdlog/spdlog.h"
#include <hpx/execution.hpp>
#include <hpx/parallel/algorithms/transform_reduce.hpp>
#include <utility>

using namespace std;
namespace par = hpx::parallel;

namespace components {

    WeekSplitter::WeekSplitter(hpx::future<hpx::naming::id_type> &&f, string basename) : client_base(move(f)),
                                                                                         _basename(
                                                                                                 std::move(basename)) {}

    WeekSplitter::WeekSplitter(hpx::naming::id_type &&f, string basename) : client_base(move(f)),
                                                                            _basename(std::move(basename)) {}

    WeekSplitter::WeekSplitter(vector<string> filenames, string basename) : client_base(
            hpx::new_<server::WeekSplitter>(hpx::find_here(), filenames)), _basename(std::move(basename)) {
        hpx::register_with_basename(_basename, get_id(), hpx::get_locality_id());
    }

    WeekSplitter::~WeekSplitter() {
        spdlog::debug("Destroying component");
        const auto f = hpx::unregister_with_basename(_basename, hpx::get_locality_id());
        hpx::wait_all(f);
    }

    hpx::future<vector<visit_row>> WeekSplitter::invoke() const {
        return hpx::async<server::WeekSplitter::invoke_action>(get_id());
    }
}
HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::component<
        components::server::WeekSplitter
> week_splitter_type;

HPX_REGISTER_COMPONENT(week_splitter_type, WeekSplitter);
HPX_REGISTER_ACTION(::components::server::WeekSplitter::invoke_action, week_splitter_invoke_action);
