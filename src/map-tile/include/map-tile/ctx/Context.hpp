//
// Created by Nicholas Robison on 7/21/20.
//

#ifndef MOBILITY_CPP_CONTEXT_HPP
#define MOBILITY_CPP_CONTEXT_HPP

#include "map-tile/coordinates/LocaleLocator.hpp"
#include <spdlog/spdlog.h>
#include <map>
#include <functional>

namespace mt::ctx {

    template<class Key, class Coordinate, class Reducer = nullptr_t>
    class Context {

    public:
        typedef std::function<void(const Coordinate &, const Key &)> emit_handler;
        typedef typename coordinates::LocaleLocator<Coordinate>::mt_tile mt_tile;

        explicit Context(const emit_handler &handler, const mt_tile &tile,
                         const std::map<std::string, std::string> &config) : _handler(handler), _tile(tile),
                                                                             _config_values(config) {

        }

        void emit(const Coordinate &coord, const Key &key) const {
            _handler(coord, key);
        }

        mt_tile get_tile() const {
            return _tile;
        }

        [[nodiscard]] std::optional<std::string> get_config_value(const std::string &key) const {
            const auto found = _config_values.find(key);
            if (found == _config_values.end()) {
                return {};
            }
            return found->second;
        }


    private:
        const emit_handler _handler;
        const mt_tile _tile;
        const std::map<std::string, std::string> _config_values;

    };

    template<class Key, class Coordinate>
    using MapContext = Context<Key, Coordinate>;
    template<class Key, class Coordinate>
    using ReduceContext = Context<Key, Coordinate>;
}

#endif //MOBILITY_CPP_CONTEXT_HPP
