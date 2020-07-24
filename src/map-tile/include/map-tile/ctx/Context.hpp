//
// Created by Nicholas Robison on 7/21/20.
//

#ifndef MOBILITY_CPP_CONTEXT_HPP
#define MOBILITY_CPP_CONTEXT_HPP

#include "../coordinates/LocaleLocator.hpp"
//#include "../MapTileClient.hpp"
#include <spdlog/spdlog.h>

namespace mt::ctx {

    template<class Key, class Reducer = nullptr_t>
    class Context {

    public:

        explicit Context(const coordinates::LocaleLocator &loc): _loc(loc) {

        }

        void emit(const coordinates::Coordinate2D &coord, const Key &key) const {
            spdlog::debug("Emitting Key");
            // Find the locale
            const auto loc = _loc.get_locale(coord);

//            const client::MapTileClient m(loc);
//            m.receive(coord, key);
        }

    private:
        const coordinates::LocaleLocator _loc;

    };

    template<class Key>
    using MapContext = Context<Key>;
    template<class Key>
    using ReduceContext = Context<Key>;
}

#endif //MOBILITY_CPP_CONTEXT_HPP
