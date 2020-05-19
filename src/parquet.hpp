#ifndef PARQUET_H
#define PARQUET_H

#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>

#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/fmt.h"



class Parquet
{
public:
    Parquet(const std::string &filename);
    std::shared_ptr<arrow::Table> read() const;
    arrow::Status write(const arrow::Table &table) const;

private:
    const std::string _filename;
};

#endif