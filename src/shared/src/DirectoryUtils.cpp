//
// Created by Nicholas Robison on 7/27/20.
//

#include "shared/DirectoryUtils.hpp"

namespace shared {


    std::vector<fs::directory_entry>
    DirectoryUtils::enumerate_files(const std::string_view input_dir, const std::string_view filter_regex) {
        // Iterate through all the files and do async things
        const boost::regex my_filter((std::string(filter_regex)));
        const auto dir_iter = fs::recursive_directory_iterator(std::string(input_dir));
        std::vector<fs::directory_entry> files;

        // We have to do this loop because the directory iterator doesn't seem to work correctly.
        for (auto &p : dir_iter) {
            // Skip if not a file
            if (!fs::is_regular_file(p.status()))
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

    fs::path DirectoryUtils::build_path(const fs::path &root_path, const std::string &path_string) {
        fs::path appender(path_string);
        if (appender.is_relative()) {
            return fs::path(root_path) /= appender;
        };
        return appender;
    };

    std::vector<std::vector<fs::directory_entry>>
    DirectoryUtils::partition_files(const std::string_view input_dir, int nl, const std::string_view filter_regex) {
        const auto files = enumerate_files(input_dir, filter_regex);

        return split_vector(files, nl);
    }

}