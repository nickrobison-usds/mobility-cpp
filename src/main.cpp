
#include <iostream>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include "parquet/arrow/reader.h"
#include <parquet/exception.h>

int main() {
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

    for(auto& c : table->schema()->fields() ) {
        std::cout << "Column: " << c->name() << std::endl;
    }

    return 0;
}