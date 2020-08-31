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
#include <memory>
#include <utility>

namespace mt::io {

    template<typename S, typename C, typename K>
    void handle_emit(std::shared_ptr<std::pair<const C, const K>> ptr, const coordinates::LocaleLocator <C> &locator) {
        const auto locale_num = locator.get_locale(ptr->first);
        auto id = hpx::find_from_basename(fmt::format("mt/base/{}", locale_num), 0).get();
        typedef typename S::receive_action action_type;
        hpx::apply<action_type>(id, ptr->first, ptr->second);
    }

    template<typename Server, typename Coordinate, typename Key>
    class EmitHandler {

    public:
        typedef typename std::pair<const Coordinate, const Key> emit_value;

        explicit EmitHandler(const coordinates::LocaleLocator <Coordinate> locator) : _locator(std::move(locator)),
                // Get the IO pool manually, so we can wait for everything to finish
                                                                                      _pool(hpx::get_runtime().get_thread_pool(
                                                                                              "io_pool")),
                                                                                      _executor(
                                                                                              hpx::parallel::execution::io_pool_executor()) {
        };

        void emit(std::shared_ptr<emit_value> value) const {
            hpx::async(_executor, &handle_emit<Server, Coordinate, Key>, value, _locator).get();
        }

    private:
        hpx::util::io_service_pool *_pool;
        hpx::parallel::execution::io_pool_executor _executor;
        const coordinates::LocaleLocator <Coordinate> _locator;
    };
}

#endif //MOBILITY_CPP_EMITHANDLER_HPP
