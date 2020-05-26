//
// Created by Nicholas Robison on 5/26/20.
//

#ifndef MOBILITY_CPP_LOCATIONJOINWRITER_HPP
#define MOBILITY_CPP_LOCATIONJOINWRITER_HPP

#include <components/data.hpp>
#include "io/parquet.hpp"

class LocationJoinWriter {

public:
    explicit LocationJoinWriter(std::string filename);

    void write(std::vector<safegraph_location> const &locations) const;


private:
    io::Parquet const _writer;
};


#endif //MOBILITY_CPP_LOCATIONJOINWRITER_HPP
