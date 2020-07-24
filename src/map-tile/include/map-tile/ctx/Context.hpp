//
// Created by Nicholas Robison on 7/21/20.
//

#ifndef MOBILITY_CPP_CONTEXT_HPP
#define MOBILITY_CPP_CONTEXT_HPP

#include "map-tile/coordinates/LocaleLocator.hpp"
#include <spdlog/spdlog.h>
#include <functional>

namespace mt::ctx {

    template<class Key,  class Reducer = nullptr_t>
    class Context {

    public:
        typedef std::function<void(const coordinates::Coordinate2D&, const Key&)> emit_handler;
        explicit Context(const emit_handler &handler): _handler(handler) {

        }

        void emit(const coordinates::Coordinate2D &coord, const Key &key) const {
            spdlog::debug("Emitting Key");
            _handler(coord, key);
        }

    private:
        const emit_handler _handler;

    };

    template<class Key>
    using MapContext = Context<Key>;
    template<class Key>
    using ReduceContext = Context<Key>;
}

#endif //MOBILITY_CPP_CONTEXT_HPP
