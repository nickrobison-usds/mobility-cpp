//
// Created by Nicholas Robison on 5/19/20.
//

#ifndef MOBILITY_CPP_WEEKSPLITTER_HPP
#define MOBILITY_CPP_WEEKSPLITTER_HPP


#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

namespace components {
    namespace server {
        class WeekSplitter : public hpx::components::component_base<WeekSplitter> {

        public:
            WeekSplitter(std::string const &filename);
            void invoke();
            HPX_DEFINE_COMPONENT_ACTION(WeekSplitter, invoke);

        private:
            std::string const file;
        };
    }

class WeekSplitter : public hpx::components::client_base<WeekSplitter, server::WeekSplitter> {

    public:
        WeekSplitter(hpx::future<hpx::naming::id_type> &&f);
        WeekSplitter(hpx::naming::id_type &&f);

        void invoke();
    };
}

HPX_REGISTER_ACTION_DECLARATION(
        ::components::server::WeekSplitter::invoke_action, week_splitter_invoke_action);


#endif //MOBILITY_CPP_WEEKSPLITTER_HPP
