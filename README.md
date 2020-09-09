# Make Mobility Fast
Try to fix some issues with our python analysis and make it more performant.

## Setup
git submodule update --init --recursive
vcpkg install geos tbb arrow range-v3 gdal

## Cluster
spack install geos tbb arrow range-v3

### Python

This repo contains a couple of helper Jupyter notebooks for testing ideas and building datasets.

Setup is based on `virtualenv`, but Conda could be used just as easily.
```bash
pip3 install virtualenv
virtualenv venv
source venv/bin/activate
pip3 install -r requirements.txt
```

Start the notebook: `jupyter notebook python/`

Some of the notebooks are described here:

#### Join locations

This notebook builds the `Joined_POI.parquet` file which is used by some of the analysis applications.
It requires the `core_poi.csv` files from Safegraph and the combined `block_groups.shp` file, which is built by combining all of the block group files from the Census bureau into a single file.

The output should be placed in the `${DATA_DIR}/reference` directory. 

#### Output Inspection

This notebook contains some helper cells for inspecting the output of the various analysis tools.