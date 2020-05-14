cmake_minimum_required(VERSION 3.0.0)
project(mobility-cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package(arrow REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(ParallelSTL CONFIG REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(mobility-cpp src/main.cpp)
target_link_libraries(mobility-cpp PRIVATE arrow parquet range-v3 fmt::fmt pstl::ParallelSTL Boost::filesystem spdlog::spdlog_header_only)
target_compile_options(mobility-cpp PRIVATE "$<$<CONFIG:RELEASE>:-O3>")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
