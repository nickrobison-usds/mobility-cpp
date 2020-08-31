//
// Created by Nicholas Robison on 8/13/20.
//

#ifndef MOBILITY_CPP_EMITHANDLER_HPP
#define MOBILITY_CPP_EMITHANDLER_HPP

#include "../coordinates/LocaleLocator.hpp"
#include <hpx/include/parallel_executors.hpp>
#include <hpx/include/runtime.hpp>
#include <hpx/io_service/io_service_pool.hpp>
#include <spdlog/fmt/fmt.h>
#include <atomic>
#include <memory>
#include <utility>

std::atomic<std::uint32_t> in_flight = 0;

namespace mt::io {

    template<typename S, typename C, typename K>
    void handle_emit(std::shared_ptr<std::pair<const C, const K>> ptr, const coordinates::LocaleLocator <C> &locator) {
        const auto locale_num = locator.get_locale(ptr->first);
        const auto id = hpx::find_from_basename(fmt::format("mt/base/{}", locale_num), 0).get();
        typedef typename S::receive_action action_type;
        try {
            spdlog::info("Emitting here");
            in_flight++;
            hpx::async<action_type>(id, ptr->first, ptr->second).get();
            spdlog::info("Emit done");
            in_flight--;
            spdlog::info("Current in flight: {}", in_flight);
        } catch (const std::exception &e) {
            spdlog::error("Unable to send value. {}", e.what());
        }
    }

    template<typename Server, typename Coordinate, typename Key>
    class EmitHandler {

    public:
        typedef typename std::pair<const Coordinate, const Key> emit_value;

        explicit EmitHandler(const coordinates::LocaleLocator <Coordinate> &locator) : _locator(locator),
                // Get the IO pool manually, so we can wait for everything to finish
                                                                                       _pool(hpx::get_runtime().get_thread_pool(
                                                                                               "io_pool")),
                                                                                       _executor(
                                                                                               hpx::parallel::execution::detail::service_executor(
                                                                                                       _pool)) {
        };

        void emit(std::shared_ptr<emit_value> value) const {
            hpx::apply(_executor, &handle_emit<Server, Coordinate, Key>, value, _locator);
        }

        void wait() const {
            _pool->wait();
        }

    private:
        hpx::util::io_service_pool *_pool;
        hpx::parallel::execution::detail::service_executor _executor;
        const coordinates::LocaleLocator <Coordinate> &_locator;
    };
}

#endif //MOBILITY_CPP_EMITHANDLER_HPP
