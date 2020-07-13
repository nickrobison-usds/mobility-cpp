//
// Created by Nicholas Robison on 5/29/20.
//

#ifndef MOBILITY_CPP_SIZER_HPP
#define MOBILITY_CPP_SIZER_HPP

#include <cstdlib>
#include <memory>

namespace io::helpers {

    template<size_t maxalign>
    struct unioner {
        template<typename T0, typename T1, size_t O>
        union U_inner {
            struct {
                char pad[O];  // offset

                T1 m[sizeof(T0) / (sizeof(T1) + 1)];  // instance of type of member
            } data;
            U_inner<T0, T1, O + 1> other;
        };

        template<typename T0, typename T1>
        union U_inner<T0, T1, 0> {
            struct {
                T1 m[sizeof(T0) / (sizeof(T1) + 1)];  // instance of type of member
            } data;
            U_inner<T0, T1, 1> other;
        };

        template<typename T0, typename T1>
        union U_inner<T0, T1, maxalign> {
        };
    };

    template<typename T0, typename T1, typename T2>
    struct offset_of_impl {
        using inner_t = typename unioner<alignof(T1)>::template U_inner<T0, T1, 0>;

        union U {
            char c;
            inner_t m;
            T0 object;

            constexpr U() : c(0) {}  // make c the active member
        };

        static constexpr U u = {};

        static constexpr const T1 *addr_helper(const T1 *base, const T1 *target) {
            auto addr = base;
            while (addr < target) {
                addr++;
            }
            return addr;
        }

        static constexpr ptrdiff_t addr_diff(const T1 *base, const T1 *target) {
            return (target - base) * sizeof(T1);
        }

        template<size_t off, typename TT>
        static constexpr std::ptrdiff_t offset2(T1 T2::*member, TT &union_part) {
            const auto addr_target =
                    std::addressof(offset_of_impl<T0, T1, T2>::u.object.*member);
            const auto addr_base = std::addressof(union_part.data.m[0]);
            const auto addr = addr_helper(addr_base, addr_target);

            // != will never return true... but < seems to work?
            if (addr < addr_target) {
                if constexpr (off + 1 < alignof(T1)) {
                    return offset2<off + 1>(member, union_part.other);
                } else {
                    throw 1;  // shouldn't happen
                }
            }
            return (addr - addr_base) * sizeof(T1) + off;
        }

        static constexpr std::ptrdiff_t offset(T1 T2::*member) {
            const auto addr_target =
                    std::addressof(offset_of_impl<T0, T1, T2>::u.object.*member);
            const auto addr_base =
                    (std::addressof(offset_of_impl<T0, T1, T2>::u.m.data.m[0]));
            const auto addr = addr_helper(addr_base, addr_target);

            return offset2<0>(member, offset_of_impl<T0, T1, T2>::u.m);

            if (addr != addr_target) {
                return 0;
            }

            return (addr - addr_base) * sizeof(T1);
        }
    };

    template<typename T0, typename T1, typename T2>
    constexpr typename offset_of_impl<T0, T1, T2>::U offset_of_impl<T0, T1, T2>::u;

    template<typename T0, typename T1, typename T2>
    inline constexpr std::ptrdiff_t offset_of(T1 T2::*member, T0 * = nullptr) {
        return offset_of_impl<T0, T1, T2>::offset(member);
    }

}

#endif //MOBILITY_CPP_SIZER_HPP
