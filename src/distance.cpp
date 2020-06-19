//
// Created by Nicholas Robison on 5/26/20.
//

#include <algorithm>
#include <io/parquet.hpp>
#include <io/Shapefile.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/parallel/executors.hpp>
#include <hpx/parallel/algorithms/transform_reduce.hpp>
#include "utils.hpp"
#include "spdlog/spdlog.h"

namespace par = hpx::parallel;

typedef std::tuple<std::string, std::string, double> distance_result;

static const char *const id_field = "GEOID";

std::vector<distance_result> compute_distance(std::string &filename) {
// Read the input file and iterate through all possible combinations, computing the distance between the CBGs
    io::Shapefile s(filename);
    GDALDatasetUniquePtr p = s.openFile();
    const auto layer = p->GetLayer(0);
    const auto count = layer->GetFeatureCount();
    spdlog::debug("Reserving {} elements", count * count);

    std::vector<OGRFeature *> features;
    features.reserve(count);
    std::transform(layer->begin(), layer->end(), std::back_inserter(features), [](const OGRFeatureUniquePtr &feature) {
        return feature->Clone();
    });

    const auto cross_join = [&features](const auto &source) {
        const auto source_id = source->GetFieldAsString(id_field);
        OGRPoint source_centroid;
        source->GetGeometryRef()->Centroid(&source_centroid);
        std::vector<distance_result> output;
        std::transform(features.begin(), features.end(), std::back_inserter(output),
                       [&source_id, &source_centroid](const auto &dest) {
                           OGRPoint dest_centroid;
                           dest->GetGeometryRef()->Centroid(&dest_centroid);
                           const auto dest_id = dest->GetFieldAsString(id_field);
                           const double distance = source_centroid.Distance(&dest_centroid);

                           return std::make_tuple(source_id, dest_id, distance);
                       });
        return output;
    };

    return par::transform_reduce(par::execution::par_unseq,
                                 features.begin(),
                                 features.end(),
                                 std::vector<std::tuple<string, string, double>>(),
                                 [](std::vector<distance_result> acc,
                                    std::vector<distance_result> v) {
                                     acc.reserve(acc.size() + v.size());
                                     move(v.begin(), v.end(), back_inserter(acc));
                                     return acc;
                                 }, cross_join);
}

int hpx_main(hpx::program_options::variables_map &vm) {
    string input_file = vm["input_file"].as<string>();
    string output_file = vm["output_file"].as<string>();

    spdlog::info("Reading file: {}", input_file);
    const auto joined = compute_distance(input_file);
    spdlog::debug("I have {} cross-joined features.", joined.size());

    // Write it to a new Parquet file
    arrow::StringBuilder source_cbg_builder;
    arrow::StringBuilder dest_cbg_builder;
    arrow::DoubleBuilder distance_builder;

    for (const auto &d : joined) {
        arrow::Status status;
        status = source_cbg_builder.Append(std::get<0>(d));
        status = dest_cbg_builder.Append(std::get<1>(d));
        status = distance_builder.Append(std::get<2>(d));
    }

    arrow::Status status;
    std::shared_ptr<arrow::Array> source_cbg_array;
    status = source_cbg_builder.Finish(&source_cbg_array);
    std::shared_ptr<arrow::Array> dest_cbg_array;
    status = dest_cbg_builder.Finish(&dest_cbg_array);
    std::shared_ptr<arrow::Array> distance_array;
    status = distance_builder.Finish(&distance_array);

    auto schema = arrow::schema(
            {arrow::field("source_cbg", arrow::utf8()),
             arrow::field("dest_cbg", arrow::utf8()),
             arrow::field("distance", arrow::float64())});

    auto data_table = arrow::Table::Make(schema, {source_cbg_array, dest_cbg_array, distance_array});

    const io::Parquet parquet_writer(output_file);

    status = parquet_writer.write(*data_table);
    if (!status.ok()) {
        spdlog::critical("Unable to write file: {}", status.CodeAsString());
    };


    return hpx::finalize();
}

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()("input_file", value<string>()->default_value("data"),
                                   "Input file to parse")("output_file",
                                                          value<string>()->default_value("./wrong.parquet"),
                                                          "output file to write");

    return hpx::init(desc_commandline, argc, argv);
}