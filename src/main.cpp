
#include <algorithm>
#include <execution>
#include <string>

#include "parquet.hpp"

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/parallel/execution.hpp>
#include <hpx/parallel/algorithms/transform_reduce.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h" // Get FMT from spdlog, to avoid conflicts with other libraries.

// using namespace ranges;
namespace par = hpx::parallel;
namespace fs = boost::filesystem;

bool IsParenthesesOrDash(char c)
{
    switch (c)
    {
    case '[':
    case ']':
    case ' ':
    case '-':
        return true;
    default:
        return false;
    }
}

std::vector<int16_t> split(const std::string &str, char delim)
{
    std::vector<int16_t> strings;
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        auto base = str.substr(start, end - start);
        base.erase(std::remove_if(base.begin(), base.end(), &IsParenthesesOrDash), base.end());
        strings.push_back(std::stoi(base));
    }
    return strings;
}

std::vector<const data_row>
TableToVector(const std::shared_ptr<arrow::Table> &table)
{
    std::vector<const data_row> rows;
    rows.reserve(table->num_rows());

    auto location_cbg = std::static_pointer_cast<arrow::StringArray>(table->column(4)->chunk(0));
    auto visit_cbg = std::static_pointer_cast<arrow::StringArray>(table->column(0)->chunk(0));
    auto date = std::static_pointer_cast<arrow::Date32Array>(table->column(1)->chunk(0));
    auto distance = std::static_pointer_cast<arrow::DoubleArray>(table->column(10)->chunk(0));
    auto visits = std::static_pointer_cast<arrow::StringArray>(table->column(2)->chunk(0));

    for (int64_t i = 0; i < table->num_rows(); i++)
    {
        const std::string cbg = location_cbg->GetString(i);
        const std::string visit = visit_cbg->GetString(i);
        const ArrowDate d2 = date->Value(i);
        const double d = distance->Value(i);
        const std::string visit_str = visits->GetString(i);
        std::vector<int16_t> visits;
        try
        {
            visits = split(visit_str, ',');
        }
        catch (const std::invalid_argument &e)
        {
            spdlog::critical("Problem doing conversion: {}\n{}", e.what(), visit_str);
        }
        rows.push_back({cbg, visit, d2, visits, d});
    }

    return rows;
}

int hpx_main(hpx::program_options::variables_map &vm)
{
    std::string input_dir = vm["input_dir"].as<std::string>();
    std::string output_file = vm["output_file"].as<std::string>();

    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Mobilizing");

    // Ok. Let's try to read from disk
    // Iterate through all the files and do async things
    const boost::regex my_filter(".*\\.parquet");
    const auto dir_iter = fs::directory_iterator(input_dir);
    std::vector<const boost::filesystem::directory_entry> files;

    // We have to do this loop because the directory iterator doesn't seem to work correctly.
    for (const auto &p : dir_iter)
    {
        // Skip if not a file
        if (!boost::filesystem::is_regular_file(p.status()))
            continue;

        boost::smatch what;
        if (!boost::regex_match(p.path().filename().string(), what, my_filter))
            continue;

        files.push_back(p);
    };

    std::vector<hpx::shared_future<std::vector<const visit_row>>> rows;

    for (const auto &f : files)
    {
        hpx::shared_future<std::vector<const data_row>> a = hpx::async([&f]() {
            const Parquet parquet_reader(f.path().string());
            auto table = parquet_reader.read();
            return TableToVector(table);
        });

        hpx::shared_future<std::vector<const visit_row>> a2 = a.then([](hpx::shared_future<std::vector<const data_row>> rf) {
            const auto rows = rf.get();
            std::vector<const visit_row> ret = par::transform_reduce(
                par::execution::seq,
                rows.begin(),
                rows.end(),
                std::vector<const visit_row>(),
                [](std::vector<const visit_row> acc, std::vector<const visit_row> v) {
                    std::move(v.begin(), v.end(), std::back_inserter(acc));
                    return acc;
                },
                [](const data_row &row) {
                    std::vector<const visit_row> out;
                    out.reserve(row.visits.size());
                    for (int i = 0; i < row.visits.size(); i++)
                    {
                        const auto visit = row.visits[i];
                        out.push_back({row.location_cbg, row.visit_cbg, row.date + 1, visit, row.distance, visit * row.distance});
                    }
                    return out;
                });
            return ret;
        });
        rows.push_back(a2);
    }

    std::vector<hpx::shared_future<std::vector<const visit_row>>> output = hpx::when_all(rows.begin(), rows.end()).get();
    spdlog::info("Finished parsing and expanding Parquet file");

    // Now, sort and group
    auto sorted = [](auto lhs, auto rhs) {
        return lhs.location_cbg.compare(rhs.location_cbg);
    };

    auto group_location = [](data_row &lhs, data_row &rhs) {
        return lhs.location_cbg == rhs.location_cbg;
    };

    // Write it to a new Parquet file
    // We want the cbg pair, the date, the total visits and the distance

    arrow::StringBuilder loc_cbg_builder;
    arrow::StringBuilder visit_cbg_builder;
    arrow::Date32Builder visit_date_builder;
    arrow::Int16Builder visit_count_builder;
    arrow::DoubleBuilder distance_builder;
    arrow::DoubleBuilder weight_builder;

    std::for_each(output.begin(), output.end(), [&loc_cbg_builder, &visit_cbg_builder, &visit_date_builder, &visit_count_builder, &distance_builder, &weight_builder](hpx::shared_future<std::vector<const visit_row>> &rf) {
        auto rows = rf.get();
        std::for_each(rows.begin(), rows.end(), [&loc_cbg_builder, &visit_cbg_builder, &visit_date_builder, &visit_count_builder, &distance_builder, &weight_builder](const visit_row &row) {
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

    auto data_table = arrow::Table::Make(schema, {loc_cbg_array, visit_cbg_array, visit_date_array, visit_count_array, distance_array});

    const Parquet parquet_writer(output_file);

    status = parquet_writer.write(*data_table);
    if (!status.ok())
    {
        spdlog::critical("Unable to write file: {}", status.CodeAsString());
    }

    return hpx::finalize();
}

int main(int argc, char **argv)
{
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()("input_dir", value<std::string>()->default_value("data"), "Input directory to parse")("output_file", value<std::string>()->default_value("./wrong.parquet"), "output file to write");

    return hpx::init(desc_commandline, argc, argv);
}