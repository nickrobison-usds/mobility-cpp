//
// Created by Nicholas Robison on 7/21/20.
//

#ifndef MOBILITY_CPP_CONTEXT_HPP
#define MOBILITY_CPP_CONTEXT_HPP

#include <iostream>
#include <spdlog/spdlog.h>

namespace mt {

    template<class Key, class Reducer = nullptr_t>
    class Context {

    public:
        template<typename = typename std::enable_if<!std::is_same_v<Reducer, nullptr_t>>>
        void emit(const Key &key) const {
            std::cout << "Emitting" << std::endl;
            spdlog::debug("Emitting Key");
        }

    };

    template<class Key>
    using MapContext = Context<Key>;
    template<class Key>
    using ReduceContext = Context<Key>;
}

#endif //MOBILITY_CPP_CONTEXT_HPP
