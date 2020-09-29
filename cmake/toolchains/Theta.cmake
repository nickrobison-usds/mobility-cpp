# Set the Cray Compiler Wrapper
set(CMAKE_CXX_COMPILER CC)
set(CMAKE_C_COMPILER cc)
set(CMAKE_Fortran_COMPILER ftn)

# Configure HPX
set(USE_INTERNAL_HPX ON CACHE BOOL "")
set(USE_INTERNAL_ARROW ON CACHE BOOL "")
set(HPX_WITH_ASYNC_MPI ON CACHE BOOL "")
set(HPX_WITH_PARCELPORT_MPI ON CACHE BOOL "")
set(HPX_WITH_PARCELPORT_MPI_MULTITHREADED OFF CACHE BOOL "")
set(HPX_WITH_MALLOC "jemalloc" CACHE STRING "")

# Configure application
set(DEFAULT_MALLOC "jemalloc" CACHE STRING "")

# Compile/link flags
set(CMAKE_C_FLAGS_INIT
        ""
        CACHE STRING ""
        )
set(CMAKE_SHARED_LIBRARY_C_FLAGS
        "-fPIC -shared"
        CACHE STRING ""
        )
set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS
        "-fPIC -shared"
        CACHE STRING ""
        )
set(CMAKE_C_COMPILE_OBJECT
        "<CMAKE_C_COMPILER> -shared -fPIC <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>"
        CACHE STRING ""
        )
set(CMAKE_C_LINK_EXECUTABLE
        "<CMAKE_C_COMPILER> -fPIC <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
        CACHE STRING ""
        )
set(CMAKE_C_CREATE_SHARED_LIBRARY
        "<CMAKE_C_COMPILER> -fPIC -shared <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES> "
        CACHE STRING ""
        )
#
set(CMAKE_CXX_FLAGS_INIT
        ""
        CACHE STRING ""
        )
set(CMAKE_SHARED_LIBRARY_CXX_FLAGS
        "-fPIC -shared"
        CACHE STRING ""
        )
set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS
        "-fPIC -shared"
        CACHE STRING ""
        )
set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS
        "-fPIC -shared"
        CACHE STRING ""
        )
set(CMAKE_CXX_COMPILE_OBJECT
        "<CMAKE_CXX_COMPILER> -shared -fPIC <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>"
        CACHE STRING ""
        )
set(CMAKE_CXX_LINK_EXECUTABLE
        "<CMAKE_CXX_COMPILER> -fPIC -dynamic <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
        CACHE STRING ""
        )
set(CMAKE_CXX_CREATE_SHARED_LIBRARY
        "<CMAKE_CXX_COMPILER> -fPIC -shared <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>"
        CACHE STRING ""
        )
#
set(CMAKE_Fortran_FLAGS_INIT
        ""
        CACHE STRING ""
        )
set(CMAKE_SHARED_LIBRARY_Fortran_FLAGS
        "-fPIC"
        CACHE STRING ""
        )

set(CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS
        "-shared"
        CACHE STRING ""
        )
set(CMAKE_Fortran_COMPILE_OBJECT
        "<CMAKE_Fortran_COMPILER> -shared -fPIC <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>"
        CACHE STRING ""
        )
set(CMAKE_Fortran_LINK_EXECUTABLE
        "<CMAKE_Fortran_COMPILER> -fPIC <FLAGS> <CMAKE_Fortran_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
        )
set(CMAKE_Fortran_CREATE_SHARED_LIBRARY
        "<CMAKE_Fortran_COMPILER> -fPIC -shared <CMAKE_SHARED_LIBRARY_Fortran_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES> "
        CACHE STRING ""
        )
#
# Disable searches in the default system paths. We are cross compiling after all
# and cmake might pick up wrong libraries that way
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

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

set(HPX_WITH_PARCELPORT_TCP
        ON
        CACHE BOOL ""
        )
set(HPX_WITH_PARCELPORT_MPI
        ON
        CACHE BOOL ""
        )
set(HPX_WITH_PARCELPORT_MPI_MULTITHREADED
        ON
        CACHE BOOL ""
        )

set(HPX_WITH_PARCELPORT_LIBFABRIC
        ON
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
set(HPX_PARCELPORT_LIBFABRIC_WITH_DEV_MODE
        OFF
        CACHE BOOL "Custom libfabric logging flag"
        )
set(HPX_PARCELPORT_LIBFABRIC_WITH_LOGGING
        OFF
        CACHE BOOL "Libfabric parcelport logging on/off flag"
        )
set(HPX_WITH_ZERO_COPY_SERIALIZATION_THRESHOLD
        "4096"
        CACHE
        STRING
        "The threshold in bytes to when perform zero copy optimizations (default: 128)"
        )