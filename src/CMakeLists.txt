set(HPX_WITH_MALLOC "jemalloc")

find_package(Arrow REQUIRED)
find_package(Boost COMPONENTS filesystem regex program_options system REQUIRED)
find_package(HPX CONFIG REQUIRED)
find_package(TCMalloc)
find_package(MPI)

# Include AVX, if it's available
include(FindAVX)
CHECK_FOR_AVX()

add_subdirectory(io)
add_subdirectory(components)
add_subdirectory(shared)

add_executable(splitter splitter.cpp)
target_link_libraries(splitter PRIVATE io components Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog hpx)
target_compile_options(splitter PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(splitter PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

add_executable(distance distance.cpp)
target_link_libraries(distance PRIVATE io components Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog hpx)
target_compile_options(distance PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(distance PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

add_executable(connectivity connectivity.cpp)
target_link_libraries(connectivity PRIVATE io components shared Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog MPI::MPI_CXX)
target_compile_options(connectivity PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(connectivity PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
add_sanitizers(connectivity)

if (${DEFAULT_MALLOC} STREQUAL "tcmalloc")
    message("Linking with TCMalloc")
    target_link_libraries(connectivity PRIVATE ${TCMALLOC_LIBRARY})
endif ()
