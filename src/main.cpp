
#include <iostream>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include "parquet/arrow/reader.h"
#include <parquet/exception.h>

struct data_row
{
    std::string location_cbg;
    std::string visit_cbg;
    // arrow::date32 date;
    std::vector<int32_t> visits;
    double distance;
};

arrow::Status
TableToVector(const std::shared_ptr<arrow::Table> &table, std::vector<struct data_row> &rows)
{

    auto location_cbg = std::static_pointer_cast<arrow::StringArray>(table->column(4)->chunk(0));
    auto visit_cbg = std::static_pointer_cast<arrow::StringArray>(table->column(0)->chunk(0));
    auto distance = std::static_pointer_cast<arrow::DoubleArray>(table->column(10)->chunk(0));
    auto visits = std::static_pointer_cast<arrow::StringArray>(table->column(2)->chunk(0));

    for (int64_t i = 0; i < table->num_rows(); i++)
    {
        std::string cbg = location_cbg->GetString(i);
        std::string visit = visit_cbg->GetString(i);
        double d = distance->Value(i);
        std::string visit_str = visits->GetString(i);
        std::vector<int32_t> visits;
        // std::cout << "CBG: " << cbg << std::endl;
        rows.push_back({cbg, visit, visits, d});
    }

    return arrow::Status::OK();
}

int main()
{
    std::cout << "Hello world" << std::endl;

    // Ok. Let's try to read from disk
    auto filename = std::string("/Users/raac/Development/covid/mobility-analysis/data/output/SG-April-weekly-summary.parquet/part.0.parquet");

    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(filename, arrow::default_memory_pool()));

    std::unique_ptr<parquet::arrow::FileReader> reader;
    PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

    std::shared_ptr<arrow::Table> table;
    PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
    std::cout << "Loaded " << table->num_rows() << " rows in " << table->num_columns()
              << " columns." << std::endl;

    for (auto &c : table->schema()->fields())
    {
        std::cout << "Column: " << c->name() << " Type: " << c->type()->ToString() << std::endl;
    }

    std::vector<struct data_row> rows;
    rows.reserve(table->num_rows());

    auto resp = TableToVector(table, rows);
    if (!resp.ok())
    {
        std::cout << "Problem! " << resp.ToString() << std::endl;
    }

    std::cout << "Rows of everything: " << rows.size() << std::endl;

    return 0;
}