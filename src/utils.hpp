//
// Created by Nicholas Robison on 5/24/20.
//

#ifndef MOBILITY_CPP_UTILS_HPP
#define MOBILITY_CPP_UTILS_HPP

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#endif //MOBILITY_CPP_UTILS_HPP

using namespace std;
namespace fs = boost::filesystem;

/**
 * Split a vector into a given number of slices
 * @tparam T - Type of vector to split
 * @param vec - Input vector to split
 * @param n - Number of partitions to split into
 * @return - A vector of vectors with size equal to `n`
 */
template<typename T>
vector<vector<T>> SplitVector(const vector<T> &vec, size_t n) {
    vector<vector<T>> outVec;

    size_t length = vec.size() / n;
    size_t remain = vec.size() % n;

    size_t begin = 0;
    size_t end = 0;

    for (size_t i = 0; i < min(n, vec.size()); ++i) {
        end += (remain > 0) ? (length + !!(remain--)) : length;

        outVec.push_back(vector<T>(vec.begin() + begin, vec.begin() + end));

        begin = end;
    }

    return outVec;
}
vector<boost::filesystem::directory_entry>
enumerate_files(string const &input_dir, string const &filter_regex) {
    // Iterate through all the files and do async things
    const boost::regex my_filter(filter_regex);
    const auto dir_iter = fs::directory_iterator(input_dir);
    vector<boost::filesystem::directory_entry> files;

    // We have to do this loop because the directory iterator doesn't seem to work correctly.
    for (auto &p : dir_iter) {
        // Skip if not a file
        if (!boost::filesystem::is_regular_file(p.status()))
            continue;

        boost::smatch what;
        if (!boost::regex_match(p.path().filename().string(), what, my_filter))
            continue;

        files.push_back(p);
    };

    // Do a lexical sort on the files
    sort(files.begin(), files.end());

    return files;
}


/**
 * For a given input directory, split all the files in `nl` number of vectors.
 * If the split is uneaven some vectors will have one more element than other vectors.
 * @param input_dir - Input directory to iterate through and split.
 * @param nl - Number of vectors to partition files into
 * @param filter_regex - Filter regex for selecting which files to include
 * @return - `NL` vectors of vectors of directory entries.
 */
vector<vector<boost::filesystem::directory_entry>>
partition_files(string const &input_dir, int nl, string const &filter_regex) {
    const auto files = enumerate_files(input_dir, filter_regex);

    return SplitVector(files, nl);
}