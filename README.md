# Make Mobility Fast
Try to fix some issues with our python analysis and make it more performant.

## Setup
git submodule update --init --recursive
vcpkg install geos tbb arrow range-v3 gdal

```bash
brew install cmake ninja gdal apache-arrow jemalloc boost openssl eigen
```

### Cluster

#### CADES
```bash
module load PE-gnu/4.0
spack env activate .
spack install
cmake -B build/
``` 

#### CORI

```bash
module swap PrgEnv-intel PrgEnv-cray
spack env activate .
spack install
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/Cori.cmake -B build/
```

#### Theta
> Note: We don't currently support building with static linking (which is the default on Theta).

```bash
export CRAYPE_LINK_TYPE=dynamic
module swap craype-mic-knl craype-haswell
module swap PrgEnv-intel PrgEnv-gnu
spack env activate .
spack install
module load cmake cray-jemalloc
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/Theta.cmake -B build/ 
```


```bash
cd ~/mobility-cpp
. /gpfs/mira-home/nickrobison/spack/share/spack/setup-env.sh
spack env activate .
```

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

## Reference datasets

The application makes use of a couple of reference datasets that need to be built, prior to launch.

### Unified Census Block file

### Joined_POI

We need to join the Safegraph Core_POI dataset with unified Census Block file generated in the previous step.

1. Download the entirety of the POI catalog from the 
2. Unzip and un-gzip the poi files.
```bash
find . -name '*.zip' -exec sh -c 'unzip -o -d "${0%.*}" "$0"' '{}' ';'
gunzip -r .
```
3. Join them all together into a single, massive set of Parquet files.