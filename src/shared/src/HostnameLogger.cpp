//
// Created by Nicholas Robison on 6/19/20.
//

#include "shared/HostnameLogger.hpp"
#include <boost/asio.hpp>

namespace shared {

    std::string get_hostname() {
        return boost::asio::ip::host_name();
    };

    HostnameLogger::HostnameLogger(): _hostname(get_hostname()) {
        // Not used
    }

    HostnameLogger::HostnameLogger(const std::string &hostname): _hostname(hostname) {
        // Not used
    }

    void HostnameLogger::format(const spdlog::details::log_msg &, const tm &, spdlog::memory_buf_t &dest) {
        dest.append(_hostname.data(), _hostname.data() + _hostname.size());
    }

    std::unique_ptr<spdlog::custom_flag_formatter> HostnameLogger::clone() const {
        return spdlog::details::make_unique<HostnameLogger>(_hostname);
    }


}
