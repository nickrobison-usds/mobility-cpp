find_package(HPX CONFIG REQUIRED)

add_executable(locales locale_main.cpp)
target_link_libraries(locales PRIVATE HPX::hpx)