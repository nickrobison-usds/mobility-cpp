//
// Created by Nicholas Robison on 7/27/20.
//

#include <iomanip>
#include <sstream>
#include "shared/DateUtils.hpp"

namespace shared {
    std::chrono::system_clock::time_point DateUtils::parse_date(const std::string &date){
        tm tm = {};
        std::stringstream ss{date};
        ss >> std::get_time(&tm, "%Y-%m-%d");
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(mktime(&tm));

        return tp;
    }

    date::sys_days DateUtils::to_days(const std::string &str, const char* fmt) {
        std::istringstream sstream{str};
        date::sys_days date;
        sstream >> date::parse(fmt, date);
        return date;
    }
}