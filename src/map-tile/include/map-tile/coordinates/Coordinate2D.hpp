//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_COORDINATE2D_HPP
#define MOBILITY_CPP_COORDINATE2D_HPP

#include <hpx/serialization/array.hpp>
#include <hpx/serialization/serialize.hpp>
#include <boost/geometry.hpp>
#include <array>

static constexpr int dimensions = 2;

namespace bg = boost::geometry;

namespace mt::coordinates {
    class Coordinate2D : public bg::model::point<std::size_t, dimensions, bg::cartesian_tag> {

    public:

        Coordinate2D() = default;

        Coordinate2D(const std::size_t &dim0, const std::size_t &dim1);

        [[nodiscard]] std::array<std::size_t, dimensions> getDimensions() const;

        [[nodiscard]] std::size_t get_dim0() const;
        [[nodiscard]] std::size_t get_dim1() const;
        void set_dim0(const std::size_t &dim0);
        void set_dim1(const std::size_t &dim1);

        bool operator==(const Coordinate2D &rhs) const;

        // HPX required serialization
        friend class hpx::serialization::access;
        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & _dim0 & _dim1;
        }

    private:
        std::size_t _dim0;
        std::size_t _dim1;
    };
}

namespace boost::geometry::traits {

//    using namespace mt::coordinates;
    template<>
    struct tag<mt::coordinates::Coordinate2D> {
        typedef point_tag type;
    };

    template<>
    struct coordinate_type<mt::coordinates::Coordinate2D> {
        typedef std::size_t type;
    };

    template<>
    struct coordinate_system<mt::coordinates::Coordinate2D> {
        typedef cs::cartesian type;
    };

    template<>
    struct dimension<mt::coordinates::Coordinate2D> : boost::mpl::int_<2> {
    };

    template<>
    struct access<mt::coordinates::Coordinate2D, 0> {
        static std::size_t get(const mt::coordinates::Coordinate2D &p) {
            return p.get_dim0();
        }

        static void set(mt::coordinates::Coordinate2D &p, const std::size_t &dim0) {
            p.set_dim0(dim0);
        }
    };

    template<>
    struct access<mt::coordinates::Coordinate2D, 1> {
        static std::size_t get(const mt::coordinates::Coordinate2D &p) {
            return p.get_dim1();
        }

        static void set(mt::coordinates::Coordinate2D &p, const std::size_t &dim1) {
            p.set_dim1(dim1);
        }
    };
}


#endif //MOBILITY_CPP_COORDINATE2D_HPP
