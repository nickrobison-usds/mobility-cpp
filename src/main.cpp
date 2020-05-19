
#include <algorithm>

#include "parquet.hpp"
#include "data.hpp"

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h" // Get FMT from spdlog, to avoid conflicts with other libraries.
#include "WeekSplitter.hpp"

namespace fs = boost::filesystem;

template<typename T>
std::vector<std::vector<T>> SplitVector(const std::vector<T> &vec, size_t n) {
    std::vector<std::vector<T>> outVec;

    size_t length = vec.size() / n;
    size_t remain = vec.size() % n;

    size_t begin = 0;
    size_t end = 0;

    for (size_t i = 0; i < std::min(n, vec.size()); ++i) {
        end += (remain > 0) ? (length + !!(remain--)) : length;

        outVec.push_back(std::vector<T>(vec.begin() + begin, vec.begin() + end));

        begin = end;
    }

    return outVec;
}

int hpx_main(hpx::program_options::variables_map &vm) {
    std::string input_dir = vm["input_dir"].as<std::string>();
    std::string output_file = vm["output_file"].as<std::string>();

    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Mobilizing");

    // Ok. Let's try to read from disk
    // Iterate through all the files and do async things
    const boost::regex my_filter(".*\\.parquet");
    const auto dir_iter = fs::directory_iterator(input_dir);
    std::vector<boost::filesystem::directory_entry> files;

    // We have to do this loop because the directory iterator doesn't seem to work correctly.
    for (auto &p : dir_iter) {
        // Skip if not a file
        if (!boost::filesystem::is_regular_file(p.status()))
            continue;

        boost::smatch what;
        if (!boost::regex_match(p.path().filename().string(), what, my_filter))
            continue;

        files.push_back(p);
    };

    auto locals = hpx::find_all_localities();
    auto splits = SplitVector(files, locals.size());
    std::vector<hpx::shared_future<std::vector<visit_row>>> rows;

    for (int i = 0; i < locals.size(); i++) {
        const auto l = locals[i];
        const auto s = splits[i];
        spdlog::debug("Launching on: {}", l);
        std::vector<std::string> f;
        std::transform(s.begin(), s.end(), std::back_inserter(f), [](const auto p) {
            return p.path().string();
        });
        const auto client = hpx::new_<components::WeekSplitter>(l, f);
        rows.emplace_back(client.invoke(hpx::launch::async));
    }

    std::vector<hpx::shared_future<std::vector<visit_row>>> output = hpx::when_all(rows.begin(), rows.end()).get();
    spdlog::info("Finished parsing and expanding Parquet file");

    // Write it to a new Parquet file
    // We want the cbg pair, the date, the total visits and the distance
    arrow::StringBuilder loc_cbg_builder;
    arrow::StringBuilder visit_cbg_builder;
    arrow::Date32Builder visit_date_builder;
    arrow::Int16Builder visit_count_builder;
    arrow::DoubleBuilder distance_builder;
    arrow::DoubleBuilder weight_builder;

    std::for_each(output.begin(), output.end(),
                  [&loc_cbg_builder, &visit_cbg_builder, &visit_date_builder, &visit_count_builder, &distance_builder, &weight_builder](
                          hpx::shared_future<std::vector<visit_row>> &rf) {
                      auto rows = rf.get();
                      std::for_each(rows.begin(), rows.end(),
                                    [&loc_cbg_builder, &visit_cbg_builder, &visit_date_builder, &visit_count_builder, &distance_builder, &weight_builder](
                                            visit_row &row) {
                                        arrow::Status status;
                                        status = loc_cbg_builder.Append(row.location_cbg);
                                        status = visit_cbg_builder.Append(row.visit_cbg);
                                        status = visit_date_builder.Append(row.date);
                                        status = visit_count_builder.Append(row.visits);
                                        status = distance_builder.Append(row.distance);
                                        status = weight_builder.Append(row.weighted_total);
                                    });
                  });

    arrow::Status status;

    std::shared_ptr<arrow::Array> loc_cbg_array;
    status = loc_cbg_builder.Finish(&loc_cbg_array);
    std::shared_ptr<arrow::Array> visit_cbg_array;
    status = visit_cbg_builder.Finish(&visit_cbg_array);
    std::shared_ptr<arrow::Array> visit_date_array;
    status = visit_date_builder.Finish(&visit_date_array);
    std::shared_ptr<arrow::Array> visit_count_array;
    status = visit_count_builder.Finish(&visit_count_array);
    std::shared_ptr<arrow::Array> distance_array;
    status = distance_builder.Finish(&distance_array);
    std::shared_ptr<arrow::Array> weight_array;
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

    const Parquet parquet_writer(output_file);

    status = parquet_writer.write(*data_table);
    if (!status.ok()) {
        spdlog::critical("Unable to write file: {}", status.CodeAsString());
    }

    return hpx::finalize();
}

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()("input_dir", value<std::string>()->default_value("data"),
                                   "Input directory to parse")("output_file",
                                                               value<std::string>()->default_value("./wrong.parquet"),
                                                               "output file to write");

    return hpx::init(desc_commandline, argc, argv);
}