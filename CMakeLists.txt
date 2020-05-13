cmake_minimum_required(VERSION 3.0.0)
project(mobility-cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package(arrow REQUIRED)

add_executable(mobility-cpp src/main.cpp)
target_link_libraries(mobility-cpp arrow parquet)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
