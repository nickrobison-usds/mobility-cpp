//
// Created by Nicholas Robison on 5/26/20.
//

#include "LocationJoinWriter.hpp"

#include <utility>
#include "spdlog/spdlog.h"

std::shared_ptr<arrow::Schema> buildSchema() {
    auto schema = arrow::schema(
            {arrow::field("safegraph_place_id", arrow::utf8()),
             arrow::field("parent_safegraph_place_id", arrow::utf8()),
             arrow::field("location_name", arrow::utf8()),
             arrow::field("naics_code", arrow::utf8()),
             arrow::field("location", arrow::utf8()),
             arrow::field("cbg", arrow::utf8())});

    return schema;
}

LocationJoinWriter::LocationJoinWriter(std::string filename) : _writer(io::Parquet(std::move(filename))) {}

void LocationJoinWriter::write(std::vector<safegraph_location> const &locations) const {
    // We probably don't need to re-alloc on each invocation
    arrow::StringBuilder safegraph_place_id_builder;
    arrow::StringBuilder parent_safegraph_place_id_builder;;
    arrow::StringBuilder location_name_builder;
    arrow::StringBuilder naics_code_builder;
    arrow::StringBuilder location_builder;
    arrow::StringBuilder cbg_builder;

    for (const auto &l : locations) {
        // We should check this
        arrow::Status status;
        status = safegraph_place_id_builder.Append(l.safegraph_place_id);
        status = parent_safegraph_place_id_builder.Append(l.parent_safegraph_place_id);
        status = location_name_builder.Append(l.location_name);
        status = naics_code_builder.Append(l.naics_code);
        status = cbg_builder.Append(l.cbg);

        std::string location_wkt;
        auto wkt_char = location_wkt.data();
        l.location.exportToWkt(&wkt_char);
        status = location_builder.Append(wkt_char);
    }
    arrow::Status status;

    std::shared_ptr<arrow::Array> place_id_array;
    status = safegraph_place_id_builder.Finish(&place_id_array);
    std::shared_ptr<arrow::Array> parent_place_id_array;
    status = parent_safegraph_place_id_builder.Finish(&parent_place_id_array);
    std::shared_ptr<arrow::Array> location_name_array;
    status = location_name_builder.Finish(&location_name_array);
    std::shared_ptr<arrow::Array> naics_array;
    status = naics_code_builder.Finish(&naics_array);
    std::shared_ptr<arrow::Array> location_array;
    status = location_builder.Finish(&location_array);
    std::shared_ptr<arrow::Array> cbg_array;
    status = cbg_builder.Finish(&cbg_array);

    const auto s2 = buildSchema();

    const auto data_table = arrow::Table::Make(s2,
                                               {place_id_array, parent_place_id_array, location_name_array, naics_array,
                                                location_array, cbg_array});

    status = _writer.write(*data_table);
    if (!status.ok()) {
        spdlog::critical("Unable to write file: {}", status.CodeAsString());
    }
}
