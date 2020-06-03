//
// Created by Nicholas Robison on 5/22/20.
//

#include "io/Shapefile.hpp"

#include <iostream>
#include <utility>
#include <mutex>
#include "spdlog/spdlog.h"

using namespace std;

once_flag f1;

namespace io {
    Shapefile::Shapefile(string filename) : _file(move(filename)) {
        call_once(f1, []() {
            spdlog::debug("Initializing GDAL driver");
            GDALAllRegister();
        });
    }

    GDALDatasetUniquePtr Shapefile::openFile() {
        GDALDatasetUniquePtr ds((GDALDataset *) GDALOpenEx(_file.c_str(), GDAL_OF_VECTOR, nullptr, nullptr,                 nullptr));
        return ds;
    }
}