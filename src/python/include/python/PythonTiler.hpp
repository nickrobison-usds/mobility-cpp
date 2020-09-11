//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_PYTHONTILER_HPP
#define MOBILITY_CPP_PYTHONTILER_HPP

#include <string_view>
#include <map-tile/ctx/Context.hpp>
#include "PythonInterpreter.hpp"

namespace mcpp::python {

    template <class Coordinate, class Value>
    class PythonTiler {

    public:
      explicit PythonTiler() = default;

      void setup(const mt::ctx::MapContext<Value, Coordinate> &ctx) {
          // Do setup here
      }



    private:
        PythonInterpreter _interpreter;
    };


}

#endif //MOBILITY_CPP_PYTHONTILER_HPP
