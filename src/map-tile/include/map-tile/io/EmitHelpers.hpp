//
// Created by Nicholas Robison on 8/31/20.
//

#ifndef MOBILITY_CPP_EMITHELPERS_HPP
#define MOBILITY_CPP_EMITHELPERS_HPP

#include <spdlog/spdlog.h>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>


namespace mt::io {

    constexpr double get_emit_buffer_size() {
#ifdef EMIT_SIZE
        return EMIT_SIZE;
#else
        return 0;
#endif
    }

    template<typename Value>
    constexpr std::size_t get_array_size() {
        return static_cast<std::size_t>(get_emit_buffer_size() / sizeof(Value));
    }

    template<typename Coordinate, typename Value>
    class EmitBuffer {

    public:
        typedef typename std::pair<const Coordinate, const Value> emit_value;
        static constexpr std::size_t array_size = get_array_size<emit_value>();

        explicit EmitBuffer(const std::size_t num_locales) : _buffer_cache(num_locales),
                                                             _buffer_cache_size(num_locales),
                                                             _buffer_locks(num_locales) {
            spdlog::info("Initializing buffer for {} locales.", num_locales);
            spdlog::info("Buffer of size: {}, holds {} values.", get_emit_buffer_size(), array_size);
        }

        std::optional<std::vector<emit_value>>
        add_to_buffer(const std::size_t locale_num, std::shared_ptr<emit_value> value) {

            std::lock_guard lock(_buffer_locks[locale_num]);
            auto &locale_array = _buffer_cache[locale_num];
            std::size_t &current_size = _buffer_cache_size[locale_num];

            if (current_size < locale_array.size()) {
                // Add to buffer
                locale_array[current_size] = value;
                current_size++;
                return std::nullopt;
            } else {
                // Copy out the buffer
                std::vector<emit_value> out_buffer;
                out_buffer.reserve(array_size);
                std::for_each(locale_array.begin(), locale_array.end(), [&out_buffer] (auto p) {
                    out_buffer.push_back(*p);
                });
                locale_array[0] = value;
                current_size = 1;
                return {out_buffer};
            }
        }

        std::vector<std::vector<emit_value>> flush() const {

            std::vector<std::vector<emit_value>> output;
            output.reserve(_buffer_cache.size());

            for(std::size_t i = 0; i < _buffer_cache.size(); i++) {
                const auto cur_size = _buffer_cache_size[i];
                std::vector<emit_value> out_buffer;
                out_buffer.reserve(cur_size);
                auto locale_array = _buffer_cache[i];
                for (std::size_t idx = 0; idx < cur_size; idx++) {
                    out_buffer.push_back(*locale_array[idx]);
                }
                output.push_back(std::move(out_buffer));
            }
            return output;
        }

    private:
        std::vector<std::size_t> _buffer_cache_size;
        std::vector<std::mutex> _buffer_locks;
        std::vector<std::array<std::shared_ptr<emit_value>, array_size>> _buffer_cache;
    };
}


#endif //MOBILITY_CPP_EMITHELPERS_HPP
