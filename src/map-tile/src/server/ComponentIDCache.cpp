//
// Created by Nicholas Robison on 8/11/20.
//

#include "map-tile/server/ComponentIDCache.hpp"
#include <hpx/runtime/basename_registration.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace mt::server::detail {

    hpx::future<hpx::naming::id_type> ComponentIDCache::lookup_id(const std::uint64_t tile_id) {
        return hpx::async([this, tile_id]() {
            const auto iter = _cache.find(tile_id);

            if (iter == _cache.end()) {
                spdlog::debug("Cache miss for locale {}", tile_id);
                auto id = hpx::find_from_basename(fmt::format("mt/base/{}", tile_id),
                                                  0).get();
                this->_m.Lock();
                _cache.emplace(std::make_pair(tile_id, id));
                this->_m.Unlock();
                return id;
            }
            spdlog::debug("Returning from ID cache");
            return iter->second;
        });
    }
}

