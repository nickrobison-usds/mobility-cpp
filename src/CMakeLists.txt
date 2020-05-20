find_package(Arrow REQUIRED)
find_package(Boost COMPONENTS filesystem regex program_options REQUIRED)
find_package(HPX CONFIG REQUIRED)

add_subdirectory(io)
add_subdirectory(components)

add_executable(splitter splitter.cpp)
target_link_libraries(splitter PRIVATE io components Boost::filesystem Boost::regex Boost::program_options spdlog::spdlog hpx)
target_compile_options(splitter PRIVATE "$<$<CONFIG:RELEASE>:-O3>")