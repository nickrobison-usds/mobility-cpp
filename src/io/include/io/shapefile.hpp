//
// Created by Nicholas Robison on 5/22/20.
//

#ifndef MOBILITY_CPP_SHAPEFILE_HPP
#define MOBILITY_CPP_SHAPEFILE_HPP

#include <string>
#include "ogrsf_frmts.h"

using namespace std;

namespace io {
    class Shapefile {
    public:
        explicit Shapefile(string filename);


        template<class T, class Converter>
        vector<T> read(Converter converter) {

            const auto file = openFile();
            // Only the first layer, for now
            const auto layer = file->GetLayer(0);

            vector<T> out;
            out.reserve(layer->GetFeatureCount());

            for (const auto &feature : layer) {
                out.push_back(converter(feature));
            }


            return out;
        }

    private:
        GDALDatasetUniquePtr openFile();
        const string _file;
    };
}


#endif //MOBILITY_CPP_SHAPEFILE_HPP
