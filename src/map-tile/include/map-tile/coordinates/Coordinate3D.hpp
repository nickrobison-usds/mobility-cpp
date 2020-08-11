//
// Created by Nicholas Robison on 7/27/20.
//

#ifndef MOBILITY_CPP_COORDINATE3D_HPP
#define MOBILITY_CPP_COORDINATE3D_HPP

#include <hpx/serialization/array.hpp>
#include <hpx/serialization/serialize.hpp>
#include <boost/geometry.hpp>
#include <spdlog/fmt/fmt.h>
#include <array>

namespace bg = boost::geometry;

namespace mt::coordinates {
    class Coordinate3D : public bg::model::point<std::size_t, 3, bg::cartesian_tag> {

    public:
        static constexpr int dimensions = 3;

        Coordinate3D() = default;

        Coordinate3D(const std::size_t &dim0, const std::size_t &dim1, const std::size_t &dim2);

        [[nodiscard]] std::array<std::size_t, 3> getDimensions() const;

        [[nodiscard]] std::size_t get_dim0() const;

        [[nodiscard]] std::size_t get_dim1() const;

        void set_dim0(const std::size_t &dim0);

        void set_dim1(const std::size_t &dim1);

        [[nodiscard]] std::size_t get_dim2() const;

        void set_dim2(const std::size_t &dim2);

        bool operator==(const Coordinate3D &rhs) const;

        bool operator<(const Coordinate3D &rhs) const;

        bool operator>=(const Coordinate3D &rhs) const;

        [[nodiscard]] bool within(const bg::model::box<Coordinate3D> &rhs) const;

        // HPX required serialization
        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int) {
            ar & _dim0 & _dim1 & _dim2;
        }

        template<typename OStream>
        friend typename std::enable_if_t<!std::is_same_v<OStream, hpx::serialization::output_archive>, OStream>&
        operator<<(OStream &os, const Coordinate3D &coord) {
            return os << fmt::format("{dim0: {}, dim1:{}, dim2: {}}", coord.get_dim0(), coord.get_dim1(),
                                     coord.get_dim2());
        }

    private:
        std::size_t _dim0;
        std::size_t _dim1;
        std::size_t _dim2;
    };
}

namespace boost::geometry::traits {

    using namespace mt::coordinates;
    template<>
    struct tag<Coordinate3D> {
        typedef point_tag type;
    };

    template<>
    struct coordinate_type<Coordinate3D> {
        typedef std::size_t type;
    };

    template<>
    struct coordinate_system<Coordinate3D> {
        typedef cs::cartesian type;
    };

    template<>
    struct dimension<Coordinate3D> : boost::mpl::int_<3> {
    };

    template<>
    struct access<Coordinate3D, 0> {
        static std::size_t get(const Coordinate3D &p) {
            return p.get_dim0();
        }

        static void set(Coordinate3D &p, const std::size_t &dim0) {
            p.set_dim0(dim0);
        }
    };

    template<>
    struct access<Coordinate3D, 1> {
        static std::size_t get(const Coordinate3D &p) {
            return p.get_dim1();
        }

        static void set(Coordinate3D &p, const std::size_t &dim1) {
            p.set_dim1(dim1);
        }
    };

    template<>
    struct access<Coordinate3D, 2> {
        static std::size_t get(const Coordinate3D &p) {
            return p.get_dim2();
        }

        static void set(Coordinate3D &p, const std::size_t &dim2) {
            p.set_dim2(dim2);
        }
    };
}


#endif //MOBILITY_CPP_COORDINATE3D_HPP
