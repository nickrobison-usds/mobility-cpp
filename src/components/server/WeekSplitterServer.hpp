//
// Created by Nicholas Robison on 5/22/20.
//

#ifndef MOBILITY_CPP_WEEKSPLITTERSERVER_HPP
#define MOBILITY_CPP_WEEKSPLITTERSERVER_HPP

#include "io/parquet.hpp"
#include "../include/components/data.hpp"

#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace components::server {
    class WeekSplitter : public hpx::components::component_base<WeekSplitter> {

    public:
        explicit WeekSplitter(vector<string> filenames);

        vector<visit_row> invoke() const;

        HPX_DEFINE_COMPONENT_ACTION(WeekSplitter, invoke);

    private:

        static vector<visit_row> handleFile(string const &filename);

        static vector<data_row> tableToVector(shared_ptr<arrow::Table> table);

        static vector<int16_t> split(string const &str, char delim);

        static bool IsParenthesesOrDash(char c);

        vector<string> const files;
    };
}

HPX_REGISTER_ACTION_DECLARATION(
        ::components::server::WeekSplitter::invoke_action, week_splitter_invoke_action);


#endif //MOBILITY_CPP_WEEKSPLITTERSERVER_HPP
