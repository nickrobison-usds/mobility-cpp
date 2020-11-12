find_package(Boost COMPONENTS filesystem regex program_options system iostreams REQUIRED)
# If we're using our own copy of HPX, we need to crate an alias target to match what find_pacakage would return
if (${USE_INTERNAL_HPX})
    add_library(HPX::hpx ALIAS hpx)
    add_library(HPX::wrap_main ALIAS wrap_main)
else ()
    find_package(HPX CONFIG REQUIRED)
endif ()

# Include AVX, if it's available
include(FindAVX)
CHECK_FOR_AVX()
find_package(Eigen3 3.3 NO_MODULE)
find_package(GDAL REQUIRED)

# Create a custom GDAL target if one isn't already defined
if (NOT TARGET GDAL::GDAL)
    message("Creating custom GDAL target")
    add_library(mcpp-gdal INTERFACE)
    target_include_directories(mcpp-gdal INTERFACE ${GDAL_INCLUDE_DIR})
    target_link_libraries(mcpp-gdal INTERFACE ${GDAL_LIBRARY})
    add_library(GDAL::GDAL ALIAS mcpp-gdal)
endif ()

# Create a custom target that defines the allocator
string(TOUPPER "${DEFAULT_MALLOC}" MALLOC_UPPER)
add_library(mcpp-malloc INTERFACE IMPORTED)

if (NOT "${MALLOC_UPPER}" STREQUAL "SYSTEM")
    if ("${MALLOC_UPPER}" STREQUAL "TCMALLOC")
        find_package(TCMalloc REQUIRED)
        message("Using TCMalloc allocator")
        target_link_libraries(mcpp-malloc INTERFACE ${TCMALLOC_LIBRARY})
    elseif ("${MALLOC_UPPER}" STREQUAL "JEMALLOC")
        find_package(Jemalloc REQUIRED)
        message("Using Jemalloc allocator")
        target_include_directories(
                mcpp-malloc INTERFACE ${JEMALLOC_INCLUDE_DIR}
                ${JEMALLOC_ADDITIONAL_INCLUDE_DIR}
        )
        target_link_libraries(
                mcpp-malloc INTERFACE ${JEMALLOC_LIBRARIES}
        )
    else ()
        message(FATAL_ERROR "Unsupported allocator: ${MALLOC_UPPER}")
    endif ()
endif ()

# Create a custom target for Arrow
add_library(mcpp-arrow INTERFACE)
# If we're building arrow locally, make sure we build it first
if (${USE_INTERNAL_ARROW})
    add_dependencies(mcpp-arrow arrow_ep)
    # Gross, but for some reason, we need this to make sure the parquet lib is found correctly
    target_link_directories(mcpp-arrow INTERFACE ${CMAKE_BINARY_DIR}/arrow/lib64 ${CMAKE_BINARY_DIR}/arrow/lib)
    target_include_directories(mcpp-arrow INTERFACE ${CMAKE_BINARY_DIR}/arrow/include)
else ()
    find_package(Arrow CONFIG REQUIRED)
    target_link_directories(mcpp-arrow INTERFACE ${ARROW_LIB_DIR} ${PARQUET_LIB_DIR})
    target_include_directories(mcpp-arrow INTERFACE ${ARROW_INCLUDE_DIR} ${PARQUET_INCLUDE_DIR})
endif ()
target_link_libraries(mcpp-arrow INTERFACE arrow parquet)

add_subdirectory(io)
add_subdirectory(components)
add_subdirectory(shared)
add_subdirectory(spatial)
add_subdirectory(map-tile)
add_subdirectory(centrality)
add_subdirectory(graph)
add_subdirectory(python)
add_subdirectory(epi-rank)
add_subdirectory(dwell-time)

#add_executable(splitter splitter.cpp)
#target_link_libraries(splitter PRIVATE io mcpp-components Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog)
#target_compile_options(splitter PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
#target_compile_options(splitter PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

add_executable(distance distance.cpp)
target_link_libraries(distance PRIVATE io mcpp-components Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog)
target_compile_options(distance PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(distance PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

add_executable(connectivity connectivity.cpp)
target_link_libraries(connectivity PRIVATE io mcpp-components shared Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog mcpp-malloc)
target_compile_options(connectivity PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(connectivity PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
add_sanitizers(connectivity)
