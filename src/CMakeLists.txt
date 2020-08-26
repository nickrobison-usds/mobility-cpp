find_package(Boost COMPONENTS filesystem regex program_options system iostreams REQUIRED)
# If we're using our own copy of HPX, we need to crate an alias target to match what find_pacakage would return
if (${USE_INTERNAL_HPX})
    add_library(HPX::hpx ALIAS hpx)
else ()
    find_package(HPX CONFIG REQUIRED)
endif ()

if (NOT ${USE_INTERNAL_ARROW})
    find_package(Arrow CONFIG REQUIRED)
endif ()

# Include AVX, if it's available
include(FindAVX)
CHECK_FOR_AVX()

add_subdirectory(io)
add_subdirectory(components)
add_subdirectory(shared)
add_subdirectory(spatial)
add_subdirectory(map-tile)
add_subdirectory(centrality)
add_subdirectory(graph)

#add_executable(splitter splitter.cpp)
#target_link_libraries(splitter PRIVATE io mcpp-components Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog)
#target_compile_options(splitter PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
#target_compile_options(splitter PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

add_executable(distance distance.cpp)
target_link_libraries(distance PRIVATE io mcpp-components Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog)
target_compile_options(distance PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(distance PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

add_executable(connectivity connectivity.cpp)
target_link_libraries(connectivity PRIVATE io mcpp-components shared Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog)
target_compile_options(connectivity PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(connectivity PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
add_sanitizers(connectivity)

if (${DEFAULT_MALLOC} STREQUAL "tcmalloc")
    find_package(TCMalloc)
    message("Linking with TCMalloc")
    target_link_libraries(connectivity PRIVATE ${TCMALLOC_LIBRARY})
endif ()
