//
// Created by Nicholas Robison on 5/19/20.
//

#ifndef MOBILITY_CPP_WEEKSPLITTER_HPP
#define MOBILITY_CPP_WEEKSPLITTER_HPP

// This needs to come before the hpx includes, in order for the serialization to work.
#include "io/parquet.hpp"
#include "data.hpp"
#include "../../server/WeekSplitterServer.hpp"

#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

using namespace std;

namespace components {
    class WeekSplitter : public hpx::components::client_base<WeekSplitter, server::WeekSplitter> {

    public:
        WeekSplitter(hpx::future<hpx::naming::id_type> &&f, string basename);

        WeekSplitter(hpx::naming::id_type &&f, string basename);

        WeekSplitter(vector<string> filenames, string basename);

        ~WeekSplitter();

        [[nodiscard]] hpx::future<vector<visit_row>> invoke() const;

    private:
        const string _basename;
    };
}

//HPX_REGISTER_ACTION_DECLARATION(
//        ::components::server::WeekSplitter::invoke_action, week_splitter_invoke_action);


#endif //MOBILITY_CPP_WEEKSPLITTER_HPP
