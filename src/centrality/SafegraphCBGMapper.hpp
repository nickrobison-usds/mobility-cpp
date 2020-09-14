//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHCBGMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHCBGMAPPER_HPP

#include <components/BaseSafegraphMapper.hpp>
#include <components/detail/CBGOffsetCalculator.hpp>
#include <shared/data.hpp>


class SafegraphCBGMapper: public components::BaseSafegraphMapper<SafegraphCBGMapper, v2> {

public:
    void setup_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx);
    void map_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx, const v2 &row) const;

private:
    std::unique_ptr<components::detail::CBGOffsetCalculator> _oc;
};


#endif //MOBILITY_CPP_SAFEGRAPHCBGMAPPER_HPP
