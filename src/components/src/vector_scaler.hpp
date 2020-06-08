//
// Created by Nicholas Robison on 6/8/20.
//

#ifndef MOBILITY_CPP_VECTOR_SCALER_HPP
#define MOBILITY_CPP_VECTOR_SCALER_HPP

#include <blaze/system/Vectorization.h>
#include <blaze/math/simd/BasicTypes.h>
#include <blaze/system/Inline.h>
#include <immintrin.h>
#include <limits.h>
#include <type_traits>
#include <typeinfo>

namespace components::detail {

    // All this code is pulled from Agner Fog's VectorClass library.
    // You can find it here https://github.com/vectorclass/version2/blob/master/vectori128.h

    // Define bit-scan-reverse function. Gives index to highest set bit = floor(log2(a))
#if defined (__GNUC__) || defined(__clang__)

    static inline uint32_t bit_scan_reverse(uint32_t a) __attribute__((pure));

    static inline uint32_t bit_scan_reverse(uint32_t a) {
        uint32_t r;
        __asm("bsrl %1, %0" : "=r"(r) : "r"(a) : );
        return r;
    }

#ifdef __x86_64__

    static inline uint32_t bit_scan_reverse(uint64_t a) {
        uint64_t r;
        __asm("bsrq %1, %0" : "=r"(r) : "r"(a) : );
        return r;
    }

#else   // 32 bit mode
    static inline uint32_t bit_scan_reverse(uint64_t a) {
    uint64_t ahi = a >> 32;
    if (ahi == 0) return bit_scan_reverse(uint32_t(a));
    else return bit_scan_reverse(uint32_t(ahi)) + 32;
}
#endif
#else
    static inline uint32_t bit_scan_reverse(uint32_t a) {
    unsigned long r;
    _BitScanReverse(&r, a);            // defined in intrin.h for MS and Intel compilers
    return r;
}
#ifdef __x86_64__
static inline uint32_t bit_scan_reverse(uint64_t a) {
    unsigned long r;
    _BitScanReverse64(&r, a);          // defined in intrin.h for MS and Intel compilers
    return r;
}
#else   // 32 bit mode
static inline uint32_t bit_scan_reverse(uint64_t a) {
    uint64_t ahi = a >> 32;
    if (ahi == 0) return bit_scan_reverse(uint32_t(a));
    else return bit_scan_reverse(uint32_t(ahi)) + 32;
}
#endif
#endif

// Same function, for compile-time constants
    constexpr int bit_scan_reverse_const(uint64_t const n) {
        if (n == 0) return -1;
        uint64_t a = n, b = 0, j = 64, k = 0;
        do {
            j >>= 1;
            k = (uint64_t) 1 << j;
            if (a >= k) {
                a >>= j;
                b += j;
            }
        } while (j > 0);
        return int(b);
    }


    template<typename T>
    class Divisor_i {
    protected:
        T multiplier;                                    // multiplier used in fast division
        __m128i shift1;                                        // shift count used in fast division
        T sign;                                          // sign of divisor
    public:
        Divisor_i() {};                                        // Default constructor
        Divisor_i(int32_t d) {                                 // Constructor with divisor
            set(d);
        }

        Divisor_i(int m, int s1,
                  int sgn) {                    // Constructor with precalculated multiplier, shift and sign
            multiplier = _mm_set1_epi32(m);
            shift1 = _mm_cvtsi32_si128(s1);
            sign = _mm_set1_epi32(sgn);
        }

        template<typename V = T>
        std::enable_if_t<std::is_same_v<V, __m128i>>
        set(int32_t d) {                                  // Set or change divisor, calculate parameters
            const int32_t d1 = ::abs(d);
            int32_t sh, m;
            if (d1 > 1) {
                sh = (int) bit_scan_reverse(
                        uint32_t(d1 - 1));  // shift count = ceil(log2(d1))-1 = (bit_scan_reverse(d1-1)+1)-1
                m = int32_t((int64_t(1) << (32 + sh)) / d1 - ((int64_t(1) << 32) - 1)); // calculate multiplier
            } else {
                m = 1;                                         // for d1 = 1
                sh = 0;
                if (d == 0) m /= d;                            // provoke error here if d = 0
                if (uint32_t(d) == 0x80000000u) {              // fix overflow for this special case
                    m = 0x80000001;
                    sh = 30;
                }
            }
            multiplier = _mm_set1_epi32(m);                    // broadcast multiplier
            shift1 = _mm_cvtsi32_si128(sh);                    // shift count
            //sign = _mm_set1_epi32(d < 0 ? -1 : 0);           // bug in VS2019, 32 bit release. Replace by this:
            if (d < 0) sign = _mm_set1_epi32(-1); else sign = _mm_set1_epi32(0);  // sign of divisor
        }

        template<typename V = T>
        std::enable_if_t<std::is_same_v<V, __m256i>>
        set(int32_t d) {                                  // Set or change divisor, calculate parameters
            const int32_t d1 = ::abs(d);
            int32_t sh, m;
            if (d1 > 1) {
                sh = (int) bit_scan_reverse(
                        uint32_t(d1 - 1));  // shift count = ceil(log2(d1))-1 = (bit_scan_reverse(d1-1)+1)-1
                m = int32_t((int64_t(1) << (32 + sh)) / d1 - ((int64_t(1) << 32) - 1)); // calculate multiplier
            } else {
                m = 1;                                         // for d1 = 1
                sh = 0;
                if (d == 0) m /= d;                            // provoke error here if d = 0
                if (uint32_t(d) == 0x80000000u) {              // fix overflow for this special case
                    m = 0x80000001;
                    sh = 30;
                }
            }
            multiplier = _mm256_set1_epi32(m);                    // broadcast multiplier
            shift1 = _mm_cvtsi32_si128(sh);                    // shift count
            //sign = _mm_set1_epi32(d < 0 ? -1 : 0);           // bug in VS2019, 32 bit release. Replace by this:
            if (d < 0) sign = _mm256_set1_epi32(-1); else sign = _mm256_set1_epi32(0);  // sign of divisor
        }

        T getm() const {                                 // get multiplier
            return multiplier;
        }

        __m128i gets1() const {                                // get shift count
            return shift1;
        }

        T getsign() const {                              // get sign of divisor
            return sign;
        }
    };

    BLAZE_ALWAYS_INLINE const blaze::SIMDint32 scale(const blaze::SIMDint32 &a, const std::int32_t scaler) noexcept
#if BLAZE_AVX2_MODE
    {
        const Divisor_i<__m256i> d(scaler);

        __m256i t1 = _mm256_mul_epi32(a.value,
                                      d.getm());               // 32x32->64 bit signed multiplication of a[0] and a[2]
        __m256i t2 = _mm256_srli_epi64(t1, 32);                   // high dword of result 0 and 2
        __m256i t3 = _mm256_srli_epi64(a.value,
                                       32);                    // get a[1] and a[3] into position for multiplication
        __m256i t4 = _mm256_mul_epi32(t3,
                                      d.getm());              // 32x32->64 bit signed multiplication of a[1] and a[3]
        __m256i t7 = _mm256_blend_epi16(t2, t4, 0xCC);
        __m256i t8 = _mm256_add_epi32(t7, a.value);                     // add
        __m256i t9 = _mm256_sra_epi32(t8, d.gets1());             // shift right arithmetic
        __m256i t10 = _mm256_srai_epi32(a.value, 31);                   // sign of a
        __m256i t11 = _mm256_sub_epi32(t10, d.getsign());         // sign of a - sign of d
        __m256i t12 = _mm256_sub_epi32(t9, t11);                  // + 1 if a < 0, -1 if d < 0
        return _mm256_xor_si256(t12, d.getsign());         // change sign if divisor negative
    }

#elif BLAZE_SSE4_MODE
    {
        const Divisor_i<__m128i> d(scaler);

        __m128i t1 = _mm_mul_epi32(a.value,
                                   d.getm());               // 32x32->64 bit signed multiplication of a[0] and a[2]
        __m128i t2 = _mm_srli_epi64(t1, 32);                   // high dword of result 0 and 2
        __m128i t3 = _mm_srli_epi64(a.value,
                                    32);                    // get a[1] and a[3] into position for multiplication
        __m128i t4 = _mm_mul_epi32(t3, d.getm());              // 32x32->64 bit signed multiplication of a[1] and a[3]
        __m128i t7 = _mm_blend_epi16(t2, t4, 0xCC);
        __m128i t8 = _mm_add_epi32(t7, a.value);                     // add
        __m128i t9 = _mm_sra_epi32(t8, d.gets1());             // shift right arithmetic
        __m128i t10 = _mm_srai_epi32(a.value, 31);                   // sign of a
        __m128i t11 = _mm_sub_epi32(t10, d.getsign());         // sign of a - sign of d
        __m128i t12 = _mm_sub_epi32(t9, t11);                  // + 1 if a < 0, -1 if d < 0
        return _mm_xor_si128(t12, d.getsign());         // change sign if divisor negative
    }

#else
    = delete;
#endif

    BLAZE_ALWAYS_INLINE const blaze::SIMDdouble scale(const blaze::SIMDdouble &v, const double scaler) noexcept

#if BLAZE_AVX2_MODE
    {
        __m256d vb = _mm256_set1_pd(scaler);
        return _mm256_div_pd(v.value, vb);
    }

#elif BLAZE_SSE4_MODE
    {
        __m128d vb = _mm_set1_pd(scaler);
        return _mm_div_pd(v.value, vb);
    }

#else
    = delete;
#endif

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
