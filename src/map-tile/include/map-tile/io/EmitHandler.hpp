//
// Created by Nicholas Robison on 8/13/20.
//

#ifndef MOBILITY_CPP_EMITHANDLER_HPP
#define MOBILITY_CPP_EMITHANDLER_HPP

#include "../coordinates/LocaleLocator.hpp"
#include "EmitHelpers.hpp"
#include <absl/synchronization/mutex.h>
#include <hpx/include/parallel_executors.hpp>
#include <hpx/include/runtime.hpp>
#include <hpx/io_service/io_service_pool.hpp>
#include <spdlog/fmt/fmt.h>
#include <memory>
#include <mutex>
#include <utility>

namespace mt::io {

    template<typename S, typename C, typename K>
    void handle_emit(std::vector<std::pair<const C, const K>> ptr,
                     const std::size_t locale_num) {
        auto id = hpx::find_from_basename(fmt::format("mt/base/{}", locale_num), 0).get();
        typedef typename S::receive_array_action action_type;
        hpx::apply<action_type>(id, ptr);
    }

    template<typename Server, typename Coordinate, typename Key>
    class EmitHandler {

    public:
        typedef typename std::pair<const Coordinate, const Key> emit_value;
        typedef typename std::shared_ptr<emit_value> ptr_value;

        static constexpr std::size_t array_size = get_array_size<emit_value>();

        explicit EmitHandler(const coordinates::LocaleLocator<Coordinate> locator) : _locator(std::move(locator)),
                // Get the IO pool manually, so we can wait for everything to finish
                                                                                     _pool(hpx::get_runtime().get_thread_pool(
                                                                                             "io_pool")),
                                                                                     _executor(
                                                                                             hpx::parallel::execution::io_pool_executor()),
                                                                                     _buffer(locator.get_num_locales()) {

        };

        void emit(std::shared_ptr<emit_value> value) {
            const auto locale_num = _locator.get_locale(value->first);
            const auto out_buffer = _buffer.add_to_buffer(locale_num, value);
            if (out_buffer.has_value()) {
                hpx::async(_executor, &handle_emit<Server, Coordinate, Key>, std::move(*out_buffer), locale_num).get();
            }
        }

        void flush() const {
            const auto vals = _buffer.flush();
            for (std::size_t i = 0; i < vals.size(); i++) {
                hpx::async(_executor, &handle_emit<Server, Coordinate, Key>, std::move(vals[i]), i).get();
            }
        }

    private:
        hpx::util::io_service_pool *_pool;
        hpx::parallel::execution::io_pool_executor _executor;
        const coordinates::LocaleLocator<Coordinate> _locator;
        EmitBuffer<Coordinate, Key> _buffer;
    };
}

#endif //MOBILITY_CPP_EMITHANDLER_HPP
