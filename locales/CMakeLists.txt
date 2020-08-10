find_package(Boost COMPONENTS filesystem regex program_options REQUIRED)

# If we're using our own copy of HPX, we need to crate an alias target to match what find_pacakage would return
if (${USE_INTERNAL_HPX})
    add_library(HPX::hpx ALIAS hpx)
else ()
    find_package(HPX CONFIG REQUIRED)
endif ()

add_executable(locales locale_main.cpp)
target_link_libraries(locales PRIVATE HPX::hpx Boost::program_options spdlog::spdlog)