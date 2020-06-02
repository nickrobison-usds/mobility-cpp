//
// Created by Nicholas Robison on 5/19/20.
//

#ifndef MOBILITY_CPP_WEEKSPLITTER_HPP
#define MOBILITY_CPP_WEEKSPLITTER_HPP

#include "io/parquet.hpp"
#include "data.hpp"
#include "../../server/WeekSplitterServer.hpp"

#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

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


#endif //MOBILITY_CPP_WEEKSPLITTER_HPP
