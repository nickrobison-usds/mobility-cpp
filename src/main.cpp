
#include <iostream>
#include <algorithm>
#include <string>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include "parquet/arrow/reader.h"
#include <parquet/exception.h>
#include <range/v3/view/group_by.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <fmt/core.h>

using namespace ranges;

using ArrowDate = arrow::Date32Type::c_type;

struct data_row
{
    const std::string location_cbg;
    const std::string visit_cbg;
    const ArrowDate date;
    std::vector<int16_t> visits;
    const double distance;
};

struct visit_row
{
    const std::string location_cbg;
    const std::string visit_cbg;
    const ArrowDate date;
    const int16_t visits;
    const double distance;
};

std::ostream &
operator<<(std::ostream &o, const data_row &dr)
{
    auto msg = fmt::format("Location: {}\n", dr.location_cbg);
    return o << msg;
}

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

arrow::Status
TableToVector(const std::shared_ptr<arrow::Table> &table, std::vector<struct data_row> &rows)
{

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
            fmt::print(stderr, "Problem doing conversion: {}\n{}\n", e.what(), visit_str);
        }
        rows.push_back({cbg, visit, d2, visits, d});
    }

    return arrow::Status::OK();
}

int main()
{
    fmt::print("Hello world.\n");

    // Ok. Let's try to read from disk
    auto filename = std::string("/Users/raac/Development/covid/mobility-analysis/data/output/SG-April-weekly-summary.parquet/part.0.parquet");

    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(filename, arrow::default_memory_pool()));

    std::unique_ptr<parquet::arrow::FileReader> reader;
    PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

    std::shared_ptr<arrow::Table> table;
    PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
    fmt::print("Loaded {} columns and {} rows.\n", table->num_columns(), table->num_rows());

    for (auto &c : table->schema()->fields())
    {
        fmt::print("Column {}. Type: {}\n", c->name(), c->type()->ToString());
    }

    std::vector<struct data_row> rows;
    rows.reserve(table->num_rows());

    auto resp = TableToVector(table, rows);
    if (!resp.ok())
    {
        fmt::print(stderr, "Problem: {}\n", resp.ToString());
    }

    fmt::print("Rows of everything: {}\n", rows.size());

    // Now, sort and group
    auto sorted = [](auto lhs, auto rhs) {
        return lhs.location_cbg.compare(rhs.location_cbg);
    };

    auto group_location = [](const data_row &lhs, const data_row &rhs) {
        return lhs.location_cbg == rhs.location_cbg;
    };

    auto grouped = rows | views::group_by(group_location); //; // | ranges::;

    auto transformed = to_vector(grouped);

    fmt::print("I have {} groups.", transformed.size());

    // Now, expand everything
    std::vector<const visit_row> output;

    for (auto g : grouped)
    {
        for (auto row : g)
        {
            for (int i = 0; i < row.visits.size(); i++)
            {
                output.push_back({row.location_cbg, row.visit_cbg, row.date + 1, row.visits[i], row.distance});
            }
        }
    }

    fmt::print("I had {} rows. Now I have {}\n", rows.size(), output.size());
    return 0;
}