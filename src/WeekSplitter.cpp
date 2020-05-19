//
// Created by Nicholas Robison on 5/19/20.
//

#include "WeekSplitter.hpp"

#include "spdlog/spdlog.h"
#include <hpx/parallel/execution.hpp>
#include <hpx/parallel/algorithms/transform_reduce.hpp>
#include <utility>

namespace par = hpx::parallel;

namespace components {
    namespace server {
        WeekSplitter::WeekSplitter(std::vector<std::string> filenames) : files(std::move(filenames)) {}

        std::vector<visit_row> WeekSplitter::invoke() const {
            spdlog::debug("Invoking week splitter on {}", this->get_id());
            return std::transform_reduce(files.begin(), files.end(), std::vector<visit_row>(), [](std::vector<visit_row> acc, std::vector<visit_row> v) {
                acc.reserve(acc.size() + v.size());
                std::move(v.begin(), v.end(), std::back_inserter(acc));
                return acc;
            }, [] (const auto f){
                return WeekSplitter::handleFile(f);
            });
        }

        std::vector<visit_row> WeekSplitter::handleFile(std::string const &filename) {
            spdlog::debug("Reading file: {}", filename);
            const auto parquet = Parquet(filename);
            const auto table = parquet.read();
            const auto rows = WeekSplitter::tableToVector(table);

            spdlog::debug("Beginning expand {} rows to {}", rows.size(), rows.size() * 7);
            return par::transform_reduce(
                    par::execution::par_unseq,
                    rows.begin(),
                    rows.end(),
                    std::vector<visit_row>(),
                    [](std::vector<visit_row> acc, std::vector<visit_row> v) {
                        acc.reserve(acc.size() + v.size());
                        std::move(v.begin(), v.end(), std::back_inserter(acc));
                        return acc;
                    },
                    [](data_row row) {
                        std::vector<visit_row> out;
                        out.reserve(row.visits.size());
                        for (int i = 0; i < row.visits.size(); i++)
                        {
                            auto visit = row.visits[i];
                            out.push_back({row.location_cbg, row.visit_cbg, row.date + 1, visit, row.distance, visit * row.distance});
                        }
                        return out;
                    });
        }

        std::vector<data_row> WeekSplitter::tableToVector(std::shared_ptr<arrow::Table> const table) {
            std::vector<data_row> rows;
            rows.reserve(table->num_rows());

            auto location_cbg = std::static_pointer_cast<arrow::StringArray>(table->column(4)->chunk(0));
            auto visit_cbg = std::static_pointer_cast<arrow::StringArray>(table->column(0)->chunk(0));
            auto date = std::static_pointer_cast<arrow::Date32Array>(table->column(1)->chunk(0));
            auto distance = std::static_pointer_cast<arrow::DoubleArray>(table->column(10)->chunk(0));
            auto visits = std::static_pointer_cast<arrow::StringArray>(table->column(2)->chunk(0));

            for (int64_t i = 0; i < table->num_rows(); i++) {
                const std::string cbg = location_cbg->GetString(i);
                const std::string visit = visit_cbg->GetString(i);
                const ArrowDate d2 = date->Value(i);
                const double d = distance->Value(i);
                const std::string visit_str = visits->GetString(i);
                std::vector<int16_t> v2;
                try {
                    v2 = WeekSplitter::split(visit_str, ',');
                }
                catch (const std::invalid_argument &e) {
                    spdlog::critical("Problem doing conversion: {}\n{}", e.what(), visit_str);
                }
                rows.push_back({cbg, visit, d2, v2, d});
            }

            return rows;
        }

        std::vector<int16_t> WeekSplitter::split(std::string const &str, char delim) {
            std::vector<int16_t> strings;
            size_t start;
            size_t end = 0;
            while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
                end = str.find(delim, start);
                auto base = str.substr(start, end - start);
                base.erase(std::remove_if(base.begin(), base.end(), &WeekSplitter::IsParenthesesOrDash), base.end());
                strings.push_back(std::stoi(base));
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

    WeekSplitter::WeekSplitter(hpx::future<hpx::naming::id_type> &&f) : client_base(std::move(f)) {}

    WeekSplitter::WeekSplitter(hpx::naming::id_type &&f) : client_base(std::move(f)) {}

   hpx::future<std::vector<visit_row>> WeekSplitter::invoke(hpx::launch::async_policy) const {
        return hpx::async<server::WeekSplitter::invoke_action>(hpx::launch::async, this->get_id());
    }
}
HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::component<
        components::server::WeekSplitter
> week_splitter_type;

HPX_REGISTER_COMPONENT(week_splitter_type, WeekSplitter);
HPX_REGISTER_ACTION(::components::server::WeekSplitter::invoke_action, week_splitter_invoke_action);
