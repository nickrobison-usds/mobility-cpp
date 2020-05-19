//
// Created by Nicholas Robison on 5/19/20.
//

#include "WeekSplitter.hpp"

namespace components {
    namespace server {
        WeekSplitter::WeekSplitter(const std::string &filename): file(filename) {

        }

        void WeekSplitter::invoke() {
            std::cout << "Hello from splitter\n" << std::endl;
        }
    }

    WeekSplitter::WeekSplitter(hpx::future<hpx::naming::id_type> &&f) : client_base(std::move(f)) {}
    WeekSplitter::WeekSplitter(hpx::naming::id_type &&f): client_base(std::move(f)) {}

    void WeekSplitter::invoke()
    {
        hpx::async<server::WeekSplitter::invoke_action>(this->get_id()).get();
    }
}
HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::component<
        components::server::WeekSplitter
> week_splitter_type;

HPX_REGISTER_COMPONENT(week_splitter_type, WeekSplitter);
HPX_REGISTER_ACTION(::components::server::WeekSplitter::invoke_action, week_splitter_invoke_action);
