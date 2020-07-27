//
// Created by Nicholas Robison on 7/27/20.
//

#ifndef MOBILITY_CPP_DIRECTORYUTILS_HPP
#define MOBILITY_CPP_DIRECTORYUTILS_HPP

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

namespace shared {


    class DirectoryUtils {
    public:
        static fs::path build_path(const fs::path &root_path, const std::string &path_string);

         /**
         * For a given input directory, split all the files in `nl` number of vectors.
         * If the split is uneaven some vectors will have one more element than other vectors.
         * @param input_dir - Input directory to iterate through and split.
         * @param nl - Number of vectors to partition files into
         * @param filter_regex - Filter regex for selecting which files to include
         * @return - `NL` vectors of vectors of directory entries.
         */
        static std::vector<std::vector<fs::directory_entry>>
        partition_files(const std::string &input_dir, int nl, const std::string &filter_regex);

        static std::vector<fs::directory_entry>
        enumerate_files(const std::string &input_dir, const std::string &filter_regex);

         /**
         * Split a vector into a given number of slices
         * @tparam T - Type of vector to split
         * @param vec - Input vector to split
         * @param n - Number of partitions to split into
         * @return - A vector of vectors with size equal to `n`
         */
        template<typename T>
        static std::vector<std::vector<T>> split_vector(const std::vector<T> &vec, size_t n) {
            std::vector<std::vector<T>> outVec;

            std::size_t length = vec.size() / n;
            std::size_t remain = vec.size() % n;

            std::size_t begin = 0;
            std::size_t end = 0;

            for (size_t i = 0; i < std::min(n, vec.size()); ++i) {
                end += (remain > 0) ? (length + !!(remain--)) : length;

                outVec.push_back(std::vector<T>(vec.begin() + begin, vec.begin() + end));

                begin = end;
            }

            return outVec;
        }
    };
}


#endif //MOBILITY_CPP_DIRECTORYUTILS_HPP
