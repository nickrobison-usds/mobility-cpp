
#include "io/parquet.hpp"
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>

#include <utility>
#include "spdlog/spdlog.h"

namespace io {


    Parquet::Parquet(std::string filename, std::size_t rows, parquet::Compression::type compressor) :
    _filename{move(filename)},
    _rows(rows),
    _compressor(compressor) {
    }

    shared_ptr<arrow::Table> Parquet::read() const {
        spdlog::info("Reading {}", this->_filename);
        std::shared_ptr<arrow::io::ReadableFile> infile;
        PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(this->_filename, arrow::default_memory_pool()));

        std::unique_ptr<parquet::arrow::FileReader> reader;
        PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

        std::shared_ptr<arrow::Table> table;
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
        // Disabling until #33 is resolved.
//        builder.compression(_compressor);
        const auto writer_props = builder.build();

        shared_ptr<arrow::io::FileOutputStream> outfile;
        PARQUET_ASSIGN_OR_THROW(
                outfile,
                arrow::io::FileOutputStream::Open(this->_filename, append));
        // We'll set the chunksize to be 10,000 rows, which is a naive default, we can tune later.
        return parquet::arrow::WriteTable(table, arrow::default_memory_pool(), outfile, _rows, writer_props);
    }
}