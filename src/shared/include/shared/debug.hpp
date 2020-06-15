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

    class DebugPoint {
    public:
        static DebugPoint create_debug_point(int code) {
            return DebugPoint(code);
        };

        void start() const {
            if constexpr(detail::debugEnabled()) {
                _start();
            } else {
                // No-op
            }
        }

        void stop() const {
            if constexpr(detail::debugEnabled()) {
                _stop();
            } else {
                // No-op
            }
        }

    private:
        explicit DebugPoint(int code) : _code(code) {
            // Not used
        }

        void _start() const {
            kdebug_signpost_start(_code, 0, 0, 0, 0);
        }

        void _stop() const {
            kdebug_signpost_end(_code, 0, 0, 0, 0);
        }

        int _code;

    };
}

#endif //MOBILITY_CPP_DEBUG_HPP
