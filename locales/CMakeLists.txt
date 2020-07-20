find_package(Boost COMPONENTS filesystem regex program_options REQUIRED)

add_executable(locales locale_main.cpp)
target_link_libraries(locales PRIVATE hpx Boost::program_options spdlog::spdlog)