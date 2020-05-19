cmake_minimum_required(VERSION 3.0.0)
project(mobility-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_subdirectory(extern/spdlog)

find_package(Arrow REQUIRED)
# find_package(range-v3 CONFIG REQUIRED)
find_package(Boost COMPONENTS filesystem regex program_options REQUIRED)
find_package(HPX CONFIG REQUIRED)

add_executable(mobility-cpp src/main.cpp src/parquet.cpp src/WeekSplitter.cpp)
target_link_libraries(mobility-cpp PRIVATE arrow parquet Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog hpx)
target_compile_options(mobility-cpp PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message("Linking to parallel stl")
    find_package(ParallelSTL CONFIG REQUIRED)
    target_link_libraries(mobility-cpp PRIVATE pstl::ParallelSTL)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
