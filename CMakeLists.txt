cmake_minimum_required(VERSION 3.0.0)
project(mobility-cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_subdirectory(extern/spdlog)
add_subdirectory(extern/CLI11)

find_package(arrow REQUIRED)
# find_package(range-v3 CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)

add_executable(mobility-cpp src/main.cpp src/parquet.cpp)
target_link_libraries(mobility-cpp PRIVATE arrow parquet TBB::tbb Boost::filesystem spdlog::spdlog CLI11::CLI11)
target_compile_options(mobility-cpp PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
