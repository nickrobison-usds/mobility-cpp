//
// Created by Nicholas Robison on 6/19/20.
//

#ifndef MOBILITY_CPP_HOSTNAMELOGGER_HPP
#define MOBILITY_CPP_HOSTNAMELOGGER_HPP

#include "spdlog/pattern_formatter.h"

namespace shared {
    class HostnameLogger : public spdlog::custom_flag_formatter {
    public:
        HostnameLogger();
        explicit HostnameLogger(const std::string &hostname);


        void format(const spdlog::details::log_msg &, const std::tm &, spdlog::memory_buf_t &dest) override;
        std::unique_ptr<custom_flag_formatter> clone() const override;

    private:
        const std::string _hostname;
    };
}


#endif //MOBILITY_CPP_HOSTNAMELOGGER_HPP
