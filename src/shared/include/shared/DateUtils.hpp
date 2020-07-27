//
// Created by Nicholas Robison on 7/27/20.
//

#ifndef MOBILITY_CPP_DATEUTILS_HPP
#define MOBILITY_CPP_DATEUTILS_HPP

#include <chrono>
#include <string>

namespace shared {
    typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>::type> days;

    class DateUtils {
        /**
         * Parse Y-M-D date from string.
         * Any additional temporal values are ignored
         * @param date - date string
         * @return - parsed date
         */
    public:
        static std::chrono::system_clock::time_point parse_date(const std::string &date);
    };
}


#endif //MOBILITY_CPP_DATEUTILS_HPP
