# Make Mobility Fast
Try to fix some issues with our python analysis and make it more performant.

## Setup
git submodule update --init --recursive

brew install mpich

vcpkg install geos tbb arrow range-v3 gdal

## Cluster
spack install geos tbb arrow range-v3
