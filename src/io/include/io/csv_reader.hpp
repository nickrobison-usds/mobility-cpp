//
// Created by Nicholas Robison on 5/23/20.
//

#ifndef MOBILITY_CPP_CSV_READER_HPP
#define MOBILITY_CPP_CSV_READER_HPP

// We rely on HPX to do our threading for us, so we'll keep things simple for now.
#define CSV_IO_NO_THREAD

#include "csv.hpp"
#include <string>

using namespace std;

namespace io {
    template<unsigned int column_count, bool skip_header>
    class CSVLoader {
    public:
        explicit CSVLoader(string filename) : _file(move(filename)) {
            // Not used
        }

        template<typename T, class Converter, typename ...Params>
        vector<T> read(Converter converter, Params &&... params) {
            CSVReader<column_count> reader(_file);
            if constexpr (skip_header)
                reader.next_line();
            vector<T> out;
            while (reader.read_row(forward<Params>(params)...)) {
                out.push_back(converter(params...));
            }
            return out;
        }

    private:
        const string _file;
    };
}

#endif //MOBILITY_CPP_CSV_READER_HPP
