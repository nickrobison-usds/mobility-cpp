//
// Created by Nicholas Robison on 6/15/20.
//

#ifndef MOBILITY_CPP_DEBUG_HPP
#define MOBILITY_CPP_DEBUG_HPP

#include "detail/debug_helpers.hpp"

#ifdef __APPLE__

#include <sys/kdebug_signpost.h>

#endif

namespace shared {

    class DebugInterval {
    public:
        static DebugInterval create_debug_point(int code) {
            return DebugInterval(code);
        };

        void start() const {
// TODO: I would love for this to be constexpr, but it's throwing errors on Linux. So I probably don't understand what's going on.
#if __APPLE__
            kdebug_signpost_start(_code, 0, 0, 0, 0);
#else
            // No op
#endif
        }

        void stop() const {
#ifdef __APPLE__
            kdebug_signpost_end(_code, 0, 0, 0, 0);
#else
            // No op
#endif
        }

    private:
        explicit DebugInterval(int code) : _code(code) {
            start();
        }

        const int _code;

    };
}

#endif //MOBILITY_CPP_DEBUG_HPP
