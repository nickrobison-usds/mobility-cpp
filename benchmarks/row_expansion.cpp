//
// Created by Nicholas Robison on 5/21/20.
//

#include <benchmark/benchmark.h>
#include <random>
#include <arrow/type.h>

// Copied, for now
using ArrowDate = arrow::Date32Type::c_type;

thread_local static std::mt19937 rng{std::random_device{}()};
thread_local static std::uniform_int_distribution<std::int16_t> pick;
thread_local static std::uniform_real_distribution<std::double_t> p2;

struct data_row {
    std::string location_cbg;
    std::string visit_cbg;
    ArrowDate date;
    std::vector<int16_t> visits;
    double distance;
};

struct visit_row {
    std::string location_cbg;
    std::string visit_cbg;
    ArrowDate date;
    int16_t visits;
    double distance;
    double weighted_total;
};

static std::string random_string(std::string::size_type length) {
    static auto &chrs = "0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::uniform_int_distribution<std::string::size_type> p3(0, sizeof(chrs) - 2);

    std::string s;

    s.reserve(length);

    while (length--)
        s += chrs[p3(rng)];

    return s;
}


static std::vector<data_row> GenerateData(const int size) {
    std::vector<data_row> rows;
    rows.reserve(size);
    for (int i = 0; i < size; i++) {
        std::vector<int16_t> visits(7);

        std::fill(visits.begin(), visits.end(), pick(rng));
        rows.push_back({random_string(7), random_string(7), 1, visits, p2(rng)});
    }

    return rows;
}

static void BM_ExpandAndCompute(benchmark::State &state) {
    for (auto _ : state) {
        state.PauseTiming();
        const auto rows = GenerateData(state.range(0));
        state.ResumeTiming();

        for (const auto &row : rows) {
            std::vector<visit_row> out;
            out.reserve(row.visits.size());
            for (int i = 0; i < row.visits.size(); i++) {
                auto visit = row.visits[i];
                out.push_back({row.location_cbg, row.visit_cbg, row.date + 1, visit, row.distance,
                               visit * row.distance});
            }
            benchmark::DoNotOptimize(out.size());
        }
        benchmark::DoNotOptimize(rows.size());
    }
}

BENCHMARK(BM_ExpandAndCompute)->DenseRange(100, 10000, 500);

static void BM_ExpandThenCompute(benchmark::State &state) {
    for (auto _ : state) {
        state.PauseTiming();
        const auto rows = GenerateData(state.range(0));
        state.ResumeTiming();

        for (const auto &row : rows) {
            std::vector<visit_row> out;
            out.reserve(row.visits.size());
            for (int i = 0; i < row.visits.size(); i++) {
                auto visit = row.visits[i];
                out.push_back({row.location_cbg, row.visit_cbg, row.date + 1, visit, row.distance,
                               0.0});
            }

            std::transform(out.begin(), out.end(), out.begin(), [](visit_row &r) {
                r.weighted_total = r.visits * r.distance;
                return r;
            });

            benchmark::DoNotOptimize(out.size());
        }
        benchmark::DoNotOptimize(rows.size());
    }
};

BENCHMARK(BM_ExpandThenCompute)->DenseRange(100, 10000, 500);