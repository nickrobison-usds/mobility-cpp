//
// Created by Nicholas Robison on 6/8/20.
//

#ifndef MOBILITY_CPP_VECTOR_SCALER_HPP
#define MOBILITY_CPP_VECTOR_SCALER_HPP

#include <blaze/math/simd/BasicTypes.h>
#include <blaze/system/Inline.h>
#include <blaze/system/Vectorization.h>
#include <immintrin.h>
#include <limits.h>

namespace components::detail {

    BLAZE_ALWAYS_INLINE const blaze::SIMDint32 scale(const blaze::SIMDint32 &v, const std::int32_t scaler) noexcept
#ifdef BLAZE_SSE2_MODE
    {
        const auto max = std::numeric_limits<short>::max();
        __m128i vb = _mm_set1_epi16(max / scaler);
        return _mm_mulhrs_epi16(v.value, vb);
    }

#else
    = delete;
#endif

    BLAZE_ALWAYS_INLINE const blaze::SIMDdouble scale(const blaze::SIMDdouble &v, const double scaler) noexcept {
        __m128d vb = _mm_set1_pd(1.0f / scaler);
        return _mm_mul_pd(v.value, vb);
    }

    template<typename T>
    struct VectorScaler {

        explicit VectorScaler(const T max) : _scaler(max) {
            // Not used
        }


        decltype(auto) operator()(const T &v) const {
            return v / _scaler;
        }

        template<typename S>
        static constexpr bool simdEnabled() {
            return true;
        }

        template<typename S>
        decltype(auto) load(const S &v) const {
            return scale(v, _scaler);
        }

        static constexpr bool paddingEnabled() { return true; }

    private:
        const T _scaler;
    };
}


#endif //MOBILITY_CPP_VECTOR_SCALER_HPP
