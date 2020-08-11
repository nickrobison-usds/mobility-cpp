//
// Created by Nicholas Robison on 8/11/20.
//

#ifndef MOBILITY_CPP_COMPONENTIDCACHE_HPP
#define MOBILITY_CPP_COMPONENTIDCACHE_HPP

#include <absl/container/flat_hash_map.h>
#include <absl/synchronization/mutex.h>
#include <hpx/future.hpp>

namespace mt::server::detail {
    class ComponentIDCache {

    public:
        explicit ComponentIDCache(const std::uint64_t locale_size) {
            _cache.reserve(locale_size);
        }

        hpx::future<hpx::naming::id_type> lookup_id(std::uint64_t tile_id);



    private:
        absl::flat_hash_map<std::uint64_t, hpx::naming::id_type> _cache;
        absl::Mutex _m;
    };
}

#endif //MOBILITY_CPP_COMPONENTIDCACHE_HPP
