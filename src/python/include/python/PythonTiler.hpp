//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_PYTHONTILER_HPP
#define MOBILITY_CPP_PYTHONTILER_HPP

#include <map-tile/ctx/Context.hpp>
#include "PandasEngine.hpp"
#include <mutex>
#include <string_view>
#include <utility>

namespace mcpp::python {

    template <class Coordinate, class Value>
    class PythonTiler {

    public:
      explicit PythonTiler() = default;

      void setup(const mt::ctx::MapContext<Value, Coordinate> &ctx) {
          // Do setup here
          _engine = std::make_unique<PandasEngine<Value>>("hello", 10'000);
      }

      void receive(const mt::ctx::ReduceContext<Value, Coordinate> &ctx, const Coordinate &key, const Value &value) {
          std::lock_guard l(_mtx);
          _engine->load(value);
      }

      void compute(const mt::ctx::ReduceContext<Value, Coordinate> &ctx) {
          _engine->evaluate();
      }

    private:
        std::mutex _mtx;
        std::unique_ptr<PandasEngine<Value>> _engine;
    };
}

#endif //MOBILITY_CPP_PYTHONTILER_HPP
