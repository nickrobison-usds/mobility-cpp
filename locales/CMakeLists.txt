find_package(HPX CONFIG REQUIRED)
find_package(Boost COMPONENTS filesystem regex program_options REQUIRED)
find_package(MPI)

add_executable(locales locale_main.cpp)
target_link_libraries(locales PRIVATE HPX::hpx Boost::program_options spdlog::spdlog MPI::MPI_CXX)