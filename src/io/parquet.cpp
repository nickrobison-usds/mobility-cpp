
#include "parquet.hpp"
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>
#include "spdlog/spdlog.h"

Parquet::Parquet(const std::string &filename) : _filename{filename}
{
}

std::shared_ptr<arrow::Table> Parquet::read() const
{
    spdlog::info("Reading {}", this->_filename);
    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(this->_filename, arrow::default_memory_pool()));

    std::unique_ptr<parquet::arrow::FileReader> reader;
    PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

    std::shared_ptr<arrow::Table> table;
    PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
    spdlog::debug("Loaded {} columns and {} rows.", table->num_columns(), table->num_rows());

    for (auto &c : table->schema()->fields())
    {
        spdlog::debug("Column {}. Type: {}", c->name(), c->type()->ToString());
    }

    return table;
}

arrow::Status Parquet::write(const arrow::Table &table) const
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(this->_filename));
    // The last argument to the function call is the size of the RowGroup in
    // the parquet file. Normally you would choose this to be rather large but
    // for the example, we use a small value to have multiple RowGroups.
    return parquet::arrow::WriteTable(table, arrow::default_memory_pool(), outfile, 3);
}