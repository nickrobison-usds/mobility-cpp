# Set the Cray Compiler Wrapper
set(CMAKE_CXX_COMPILER CC)
set(CMAKE_C_COMPILER cc)
set(CMAKE_Fortran_COMPILER ftn)

# Configure HPX
set(USE_INTERNAL_HPX ON CACHE BOOL "")
set(HPX_WITH_ASYNC_MPI ON CACHE BOOL "")
set(HPX_WITH_PARCELPORT_MPI ON CACHE BOOL "")
set(HPX_WITH_PARCELPORT_MPI_MULTITHREADED OFF CACHE BOOL "")
# TODO: Ideally, this would be enabled, but for some reason, loading the library causes the compiler to fail.
set(HPX_WITH_PARCELPORT_LIBFABRIC
        OFF
        CACHE BOOL ""
        )
set(HPX_PARCELPORT_LIBFABRIC_PROVIDER
        "gni"
        CACHE STRING "See libfabric docs for details, gni,verbs,psm2 etc etc"
        )
set(HPX_PARCELPORT_LIBFABRIC_THROTTLE_SENDS
        "256"
        CACHE STRING "Max number of messages in flight at once"
        )
set(HPX_WITH_ZERO_COPY_SERIALIZATION_THRESHOLD
        "4096"
        CACHE
        STRING
        "The threshold in bytes to when perform zero copy optimizations (default: 128)"
        )
# We have a bunch of cores on the MIC ... increase the default
set(HPX_WITH_MAX_CPU_COUNT
        "512"
        CACHE STRING ""
        )

# We do a cross compilation here ...
set(CMAKE_CROSSCOMPILING
        ON
        CACHE BOOL ""
        )

# RDTSCP is available on Xeon/Phis
set(HPX_WITH_RDTSCP
        ON
        CACHE BOOL ""
        )

set(HPX_WITH_MALLOC "jemalloc" CACHE STRING "")

# Configure application
set(DEFAULT_MALLOC "jemalloc" CACHE STRING "")