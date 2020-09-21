//
// Created by Nicholas Robison on 9/10/20.
//

#ifndef MOBILITY_CPP_PYTHONTILER_HPP
#define MOBILITY_CPP_PYTHONTILER_HPP

#include "PandasEngine.hpp"
#include <map-tile/ctx/Context.hpp>
#include <spdlog/spdlog.h>
#include <mutex>
#include <string_view>
#include <utility>

namespace mcpp::python {

    template <class Coordinate, class Value>
    class PythonTiler {

    public:
      explicit PythonTiler() = default;

      void setup(const mt::ctx::MapContext<Value, Coordinate> &ctx) {
          spdlog::info("Setting up Python tiler");
          // Do setup here
          const auto module = ctx.get_config_value("python_module");
          spdlog::info("Starting Pandas");
          _engine = std::make_unique<PandasEngine<Value>>(*module, 10'000);
      }

      void receive(const mt::ctx::ReduceContext<Value, Coordinate> &ctx, const Coordinate &key, const Value &value) {
          std::lock_guard l(_mtx);
          _engine->load(value);
      }

      void compute(const mt::ctx::ReduceContext<Value, Coordinate> &ctx) {
          try {
              _engine->evaluate();
          } catch (const std::exception &e) {
              spdlog::error("Unable to evaluate: {}", e.what());
          }
      }

    private:
        std::mutex _mtx;
        std::unique_ptr<PandasEngine<Value>> _engine;
    };
}

#endif //MOBILITY_CPP_PYTHONTILER_HPP
