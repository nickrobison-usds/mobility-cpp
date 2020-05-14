
#include <iostream>
#include <algorithm>
#include <execution>
#include <string>

#include "parquet.hpp"

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
// #include <range/v3/view/group_by.hpp>
// #include <range/v3/view/all.hpp>
// #include <range/v3/action/sort.hpp>
// #include <range/v3/range/conversion.hpp>
#include <CLI/CLI.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h" // Get FMT from spdlog, to avoid conflicts with other libraries.

// using namespace ranges;
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
    // Iterate through all the files and dump them to a vector, so we can parallel reduce them
    const auto dir_iter = fs::directory_iterator(input_dir);
    std::vector<const boost::filesystem::directory_entry> files;
    for (const auto &p : dir_iter)
    {
        files.push_back(p);
    };

    std::vector<const data_row> rows = std::transform_reduce(
        pstl::execution::par_unseq,
        files.begin(),
        files.end(),
        std::vector<const data_row>(),
        [](std::vector<const data_row> acc, std::vector<const data_row> v) {
            std::move(v.begin(), v.end(), std::back_inserter(acc));
            return acc;
        },
        [](const auto &p) {
            const Parquet parquet_reader(p.path().string());
            auto table = parquet_reader.read();

            // spdlog::debug("Reserving an additional {} rows.", table->num_rows());
            // rows.reserve(rows.size() + table->num_rows());
            return TableToVector(table);
        });

    // for (auto &p :)
    // {
    //     const Parquet parquet_reader(p.path().string());
    //     auto table = parquet_reader.read();

    //     spdlog::debug("Reserving an additional {} rows.", table->num_rows());
    //     rows.reserve(rows.size() + table->num_rows());

    //     auto resp = TableToVector(table, rows);
    //     if (!resp.ok())
    //     {
    //         spdlog::critical("Problem: {}\n", resp.ToString());
    //     }
    // }

    spdlog::info("Rows of everything: {}\n", rows.size());

    // Now, sort and group
    auto sorted = [](auto lhs, auto rhs) {
        return lhs.location_cbg.compare(rhs.location_cbg);
    };

    auto group_location = [](data_row &lhs, data_row &rhs) {
        return lhs.location_cbg == rhs.location_cbg;
    };

    // auto grouped = rows | views::group_by(group_location); //; // | ranges::;

    // auto transformed = to_vector(grouped);

    // fmt::print("I have {} groups.", transformed.size());

    // Now, expand everything
    std::vector<visit_row> output = std::transform_reduce(
        std::execution::par_unseq,
        rows.begin(),
        rows.end(),
        std::vector<visit_row>(),
        [](std::vector<visit_row> acc, std::vector<visit_row> v) {
            std::move(v.begin(), v.end(), std::back_inserter(acc));
            return acc;
        },
        [](data_row &row) {
            std::vector<visit_row> out;
            out.reserve(row.visits.size());
            for (int i = 0; i < row.visits.size(); i++)
            {
                const auto visit = row.visits[i];
                out.push_back({row.location_cbg, row.visit_cbg, row.date + 1, visit, row.distance, visit * row.distance});
            }
            return out;
        });

    spdlog::info("Expanded {} rows to {} rows.", rows.size(), output.size());

    // Write it to a new Parquet file
    // We want the cbg pair, the date, the total visits and the distance

    arrow::StringBuilder loc_cbg_builder;
    arrow::StringBuilder visit_cbg_builder;
    arrow::Date32Builder visit_date_builder;
    arrow::Int16Builder visit_count_builder;
    arrow::DoubleBuilder distance_builder;
    arrow::DoubleBuilder weight_builder;

    std::for_each(output.begin(), output.end(), [&loc_cbg_builder, &visit_cbg_builder, &visit_date_builder, &visit_count_builder, &distance_builder, &weight_builder](const visit_row &row) {
        arrow::Status status;
        status = loc_cbg_builder.Append(row.location_cbg);
        status = visit_cbg_builder.Append(row.visit_cbg);
        status = visit_date_builder.Append(row.date);
        status = visit_count_builder.Append(row.visits);
        status = distance_builder.Append(row.distance);
        status = weight_builder.Append(row.weighted_total);
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