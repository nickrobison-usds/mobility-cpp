//
// Created by Nicholas Robison on 8/13/20.
//

#ifndef MOBILITY_CPP_EMITHANDLER_HPP
#define MOBILITY_CPP_EMITHANDLER_HPP

#include "../coordinates/LocaleLocator.hpp"
#include <hpx/include/parallel_executors.hpp>
#include <spdlog/fmt/fmt.h>
#include <memory>
#include <utility>

namespace mt::io {

    template <typename S, typename C, typename K>
    void handle_emit(std::shared_ptr<std::pair<const C, const K>> ptr, const coordinates::LocaleLocator<C> &locator) {
        const auto locale_num = locator.get_locale(ptr->first);
        const auto id = hpx::find_from_basename(fmt::format("mt/base/{}", locale_num), 0).get();
        typedef typename S::receive_action action_type;
        try {
            hpx::async<action_type>(id, ptr->first, ptr->second).get();
        } catch (const std::exception &e) {
            spdlog::error("Unable to send value. {}", e.what());
        }
    }

    template <typename Server, typename Coordinate, typename Key>
    class EmitHandler {

    public:
        typedef typename std::pair<const Coordinate, const Key> emit_value;
        // Default constructors don't work here, we need to be explicit
        EmitHandler() {
            // Not used
        };

        void emit(std::shared_ptr<emit_value> value) const {
            hpx::apply(_pool, &handle_emit<Server, Coordinate, Key>, value, _locator);
        }


    private:
        const hpx::parallel::execution::io_pool_executor _pool;
        const coordinates::LocaleLocator<Coordinate> _locator;
    };
}

#endif //MOBILITY_CPP_EMITHANDLER_HPP
