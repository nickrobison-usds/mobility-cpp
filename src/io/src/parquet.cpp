
#include "io/parquet.hpp"
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include <utility>
#include "spdlog/spdlog.h"

using namespace std;

namespace io {


    Parquet::Parquet(string filename) :
    _filename{move(filename)} {
    }

    shared_ptr<arrow::Table> Parquet::read() const {
        spdlog::info("Reading {}", this->_filename);
        shared_ptr<arrow::io::ReadableFile> infile;
        PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(this->_filename, arrow::default_memory_pool()));

        unique_ptr<parquet::arrow::FileReader> reader;
        PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

        shared_ptr<arrow::Table> table;
        PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
        spdlog::debug("Loaded {} columns and {} rows.", table->num_columns(), table->num_rows());

        for (auto &c : table->schema()->fields()) {
            spdlog::debug("Column {}. Type: {}", c->name(), c->type()->ToString());
        }

        return table;
    }

    arrow::Status Parquet::write(const arrow::Table &table, bool append) const {
        parquet::WriterProperties::Builder builder;
        // Enable Snappy compression, to make things smaller
        builder.compression(parquet::Compression::SNAPPY);
        const auto writer_props = builder.build();

        shared_ptr<arrow::io::FileOutputStream> outfile;
        PARQUET_ASSIGN_OR_THROW(
                outfile,
                arrow::io::FileOutputStream::Open(this->_filename, append));
        // We'll set the chunksize to be 10,000 rows, which is a naive default, we can tune later.
        return parquet::arrow::WriteTable(table, arrow::default_memory_pool(), outfile, 10'000, writer_props);
    }
}