//
// Created by Nicholas Robison on 7/20/20.
//

#include "FileProvider.hpp"
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

namespace mt::io {

    // String specialization of FileProvider

    template<>
    std::vector<std::string> FileProvider<std::string>::provide() const {
        std::vector<std::string> output;

        using boost::iostreams::mapped_file_source;
        using boost::iostreams::stream;

        mapped_file_source mmap(_filename);
        stream <mapped_file_source> is(mmap, std::ios::binary);

        std::string line;

        while (std::getline(is, line)) {
            output.push_back(line);
        }

        return output;
    }
}