# Make Mobility Fast
Try to fix some issues with our python analysis and make it more performant.

## Setup
```bash
git submodule update --init --recursive

```

### Local machine

You'll need to install HPX and HDF5 to make use of MPI

```bash
brew install hwloc boost openmpi
```

> Don't use MPICH on Mac OS, it statically links its own version of hwloc and segfaults.

## Cluster

### CADES

```bash
module load gcc/9.2.0
module load mpich
cmake ..
```

### CORI
```bash
module swap PrgEnv-intel PrgEnv-cray
 cmake -DCMAKE_CXX_COMPILER=CC -DCMAKE_C_COMPILER=cc -DHPX_IGNORE_COMPILER_COMPATIBILITY=ON ..
```

