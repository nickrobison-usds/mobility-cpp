//
// Created by Nicholas Robison on 7/20/20.
//

#include "map-tile/io/FileProvider.hpp"
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

namespace fs = boost::filesystem;

namespace mt::io {

    // String specialization of FileProvider

    template<>
    std::vector<std::string> FileProvider<std::string>::provide() const {
        // make it a path
        const fs::path p(_filename);
        const fs::path full_path = fs::absolute(p);
        std::vector<std::string> output;

        using boost::iostreams::mapped_file_source;
        using boost::iostreams::stream;

        try {
            mapped_file_source mmap(p);
            stream<mapped_file_source> is(mmap, std::ios::binary);

            std::string line;

            while (std::getline(is, line)) {
                output.push_back(line);
            }

        } catch (const std::exception &e) {
            e.what();
        }

        return output;
    }
}