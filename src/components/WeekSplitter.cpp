//
// Created by Nicholas Robison on 5/19/20.
//
#include "WeekSplitter.hpp"

#include "spdlog/spdlog.h"
#include <hpx/parallel/execution.hpp>
#include <hpx/parallel/algorithms/transform_reduce.hpp>
#include <utility>

using namespace std;
namespace par = hpx::parallel;

namespace components {
    namespace server {
        WeekSplitter::WeekSplitter(vector<string> filenames) : files(move(filenames)) {}

        vector<visit_row> WeekSplitter::invoke() const {
            spdlog::debug("Invoking week splitter on {}", hpx::get_locality_id());
            return par::transform_reduce(
                    par::execution::par_unseq,
                    files.begin(),
                    files.end(),
                    vector<visit_row>(),
                    [](vector<visit_row> acc, vector<visit_row> v) {
                        acc.reserve(acc.size() + v.size());
                        move(v.begin(), v.end(), back_inserter(acc));
                        return acc;
                    }, [](const auto f) {
                        return WeekSplitter::handleFile(f);
                    });
        }

        vector<visit_row> WeekSplitter::handleFile(string const &filename) {
            spdlog::debug("Reading file: {}", filename);
            const auto parquet = Parquet(filename);
            const auto table = parquet.read();
            const auto rows = WeekSplitter::tableToVector(table);

            spdlog::debug("Beginning expand {} rows to {}", rows.size(), rows.size() * 7);
            return par::transform_reduce(
                    par::execution::seq,
                    rows.begin(),
                    rows.end(),
                    vector<visit_row>(),
                    [](vector<visit_row> acc, vector<visit_row> v) {
                        acc.reserve(acc.size() + v.size());
                        move(v.begin(), v.end(), back_inserter(acc));
                        return acc;
                    },
                    [](data_row row) {
                        vector<visit_row> out;
                        out.reserve(row.visits.size());
                        for (int i = 0; i < row.visits.size(); i++) {
                            auto visit = row.visits[i];
                            out.push_back({row.location_cbg, row.visit_cbg, row.date + 1, visit, row.distance,
                                           visit * row.distance});
                        }
                        return out;
                    });
        }

        vector<data_row> WeekSplitter::tableToVector(shared_ptr<arrow::Table> const table) {
            vector<data_row> rows;
            rows.reserve(table->num_rows());

            auto location_cbg = static_pointer_cast<arrow::StringArray>(table->column(4)->chunk(0));
            auto visit_cbg = static_pointer_cast<arrow::StringArray>(table->column(0)->chunk(0));
            auto date = static_pointer_cast<arrow::Date32Array>(table->column(1)->chunk(0));
            auto distance = static_pointer_cast<arrow::DoubleArray>(table->column(10)->chunk(0));
            auto visits = static_pointer_cast<arrow::StringArray>(table->column(2)->chunk(0));

            for (int64_t i = 0; i < table->num_rows(); i++) {
                const string cbg = location_cbg->GetString(i);
                const string visit = visit_cbg->GetString(i);
                const ArrowDate d2 = date->Value(i);
                const double d = distance->Value(i);
                const string visit_str = visits->GetString(i);
                vector<int16_t> v2;
                try {
                    v2 = WeekSplitter::split(visit_str, ',');
                }
                catch (const invalid_argument &e) {
                    spdlog::critical("Problem doing conversion: {}\n{}", e.what(), visit_str);
                }
                rows.push_back({cbg, visit, d2, v2, d});
            }

            return rows;
        }

        vector<int16_t> WeekSplitter::split(string const &str, char delim) {
            vector<int16_t> strings;
            size_t start;
            size_t end = 0;
            while ((start = str.find_first_not_of(delim, end)) != string::npos) {
                end = str.find(delim, start);
                auto base = str.substr(start, end - start);
                base.erase(remove_if(base.begin(), base.end(), &WeekSplitter::IsParenthesesOrDash), base.end());
                strings.push_back(stoi(base));
            }
            return strings;
        }

        template<typename Archive>
        void WeekSplitter::serialize(Archive &ar, const unsigned int version) const {
            return ar & files;
        }

        bool WeekSplitter::IsParenthesesOrDash(char c) {
            switch (c) {
                case '[':
                case ']':
                case ' ':
                case '-':
                    return true;
                default:
                    return false;
            }
        }
    }

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
