//
// Created by Nicholas Robison on 7/20/20.
//

#ifndef MOBILITY_CPP_FILEPROVIDER_HPP
#define MOBILITY_CPP_FILEPROVIDER_HPP

#include <string>
#include <vector>

namespace mt::io {

    template<typename InputKey>
    class FileProvider {
    public:
        explicit FileProvider(const std::string &filename) : _filename(filename) {
            // Not used;
        }

        std::vector<InputKey> provide() const;

    private:
        const std::string _filename;
    };
}


#endif //MOBILITY_CPP_FILEPROVIDER_HPP
