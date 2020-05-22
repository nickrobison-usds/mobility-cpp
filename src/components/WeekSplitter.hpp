//
// Created by Nicholas Robison on 5/19/20.
//

#ifndef MOBILITY_CPP_WEEKSPLITTER_HPP
#define MOBILITY_CPP_WEEKSPLITTER_HPP

// This needs to come before the hpx includes, in order for the serialization to work.
#include "io/parquet.hpp"
#include "data.hpp"

#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

using namespace std;

namespace components {
    namespace server {
        class WeekSplitter : public hpx::components::component_base<WeekSplitter> {

        public:
            explicit WeekSplitter(vector<string> filenames);

            vector<visit_row> invoke() const;

            HPX_DEFINE_COMPONENT_ACTION(WeekSplitter, invoke);

        private:
            // Serialization support: even if all of the code below runs on one
            // locality only, we need to provide an (empty) implementation for the
            // serialization as all arguments passed to actions have to support this.
            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive &ar, unsigned int version) const;

            static vector<visit_row> handleFile(string const &filename);

            static vector<data_row> tableToVector(shared_ptr<arrow::Table> table);

            static vector<int16_t> split(string const &str, char delim);

            static bool IsParenthesesOrDash(char c);

            vector<string> const files;
        };
    }

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

HPX_REGISTER_ACTION_DECLARATION(
        ::components::server::WeekSplitter::invoke_action, week_splitter_invoke_action);


#endif //MOBILITY_CPP_WEEKSPLITTER_HPP
