
#include "io/parquet.hpp"
#include "shared/include/shared/data.hpp"
#include "components/WeekSplitter.hpp"
#include "utils.hpp"

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <boost/filesystem.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h" // Get FMT from spdlog, to avoid conflicts with other libraries.

#include <algorithm>

namespace fs = boost::filesystem;
using namespace std;

const char *scatter_basename = "/mobility/weekly/scatter/";
const char *gather_basename = "/mobility/weekly/gather/";

HPX_REGISTER_GATHER(vector<visit_row>, visit_gatherer);

void gather_here(const string &output_file, hpx::future<vector<visit_row>> &result, const int nl) {
    hpx::future<vector<vector<visit_row>>> overall_result = hpx::lcos::gather_here(gather_basename, move(result), nl);
    vector<vector<visit_row>> output = overall_result.get();
    spdlog::info("Have solutions from {} locales\n", nl);

    // Write it to a new Parquet file
    // We want the cbg pair, the date, the total visits and the distance
    arrow::StringBuilder loc_cbg_builder;
    arrow::StringBuilder visit_cbg_builder;
    arrow::Date32Builder visit_date_builder;
    arrow::Int16Builder visit_count_builder;
    arrow::DoubleBuilder distance_builder;
    arrow::DoubleBuilder weight_builder;

    for_each(output.begin(), output.end(),
             [&loc_cbg_builder, &visit_cbg_builder, &visit_date_builder, &visit_count_builder, &distance_builder, &weight_builder](
                     vector<visit_row> &rows) {
                 for_each(rows.begin(), rows.end(),
                          [&loc_cbg_builder, &visit_cbg_builder, &visit_date_builder, &visit_count_builder, &distance_builder, &weight_builder](
                                  visit_row &row) {
                              arrow::Status status;
                              status = loc_cbg_builder.Append(row.location_cbg);
                              status = visit_cbg_builder.Append(row.visit_cbg);
                              status = visit_date_builder.Append(row.date.time_since_epoch().count());
                              status = visit_count_builder.Append(row.visits);
                              status = distance_builder.Append(row.distance);
                              status = weight_builder.Append(row.weighted_total);
                          });
             });

    arrow::Status status;

    shared_ptr<arrow::Array> loc_cbg_array;
    status = loc_cbg_builder.Finish(&loc_cbg_array);
    shared_ptr<arrow::Array> visit_cbg_array;
    status = visit_cbg_builder.Finish(&visit_cbg_array);
    shared_ptr<arrow::Array> visit_date_array;
    status = visit_date_builder.Finish(&visit_date_array);
    shared_ptr<arrow::Array> visit_count_array;
    status = visit_count_builder.Finish(&visit_count_array);
    shared_ptr<arrow::Array> distance_array;
    status = distance_builder.Finish(&distance_array);
    shared_ptr<arrow::Array> weight_array;
    status = weight_builder.Finish(&weight_array);

    auto schema = arrow::schema(
            {arrow::field("location_cbg", arrow::utf8()),
             arrow::field("visit", arrow::utf8()),
             arrow::field("visit_date", arrow::date32()),
             arrow::field("visit_count", arrow::int16()),
             arrow::field("distance", arrow::float64()),
             arrow::field("weighted_total", arrow::float64())});

    auto data_table = arrow::Table::Make(schema, {loc_cbg_array, visit_cbg_array, visit_date_array, visit_count_array,
                                                  distance_array, weight_array});

    const io::Parquet parquet_writer(output_file);

    status = parquet_writer.write(*data_table);
    if (!status.ok()) {
        spdlog::critical("Unable to write file: {}", status.CodeAsString());
    }
}

int hpx_main(hpx::program_options::variables_map &vm) {
    string input_dir = vm["input_dir"].as<string>();
    string output_file = vm["output_file"].as<string>();

    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Mobilizing");

    // Ok. Let's try to read from disk
    auto locales = hpx::find_all_localities();
    const auto locale_id = hpx::get_locality_id();
    const auto partitioned = partition_files(input_dir, locales.size(), ".*\\.parquet");
    spdlog::debug("Running on locale {} of {}.", locale_id, locales.size());

    // Create a component on each locale to handle the given file list.

    const auto s = partitioned.at(locale_id);
    vector<string> f;
    transform(s.begin(), s.end(), back_inserter(f), [](const auto p) {
        return p.path().string();
    });

    const auto client = components::WeekSplitter(f, scatter_basename);
    hpx::future<vector<visit_row>> result = client.invoke();

    if (0 == locale_id) {
        gather_here(output_file, result, locales.size());
    } else {
        hpx::lcos::gather_there(gather_basename, move(result)).wait();
    }
    return hpx::finalize();
}

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()("input_dir", value<string>()->default_value("data"),
                                   "Input directory to parse")("output_file",
                                                               value<string>()->default_value("./wrong.parquet"),
                                                               "output file to write");

    std::vector<std::string> const cfg = {
            "hpx.run_hpx_main!=1"
    };
    return hpx::init(desc_commandline, argc, argv, cfg);
}