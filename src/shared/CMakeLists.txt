add_library(shared src/HostnameLogger.cpp include/shared/HostnameLogger.hpp src/DateUtils.cpp include/shared/DateUtils.hpp)

target_link_libraries(shared PUBLIC spdlog date PRIVATE Boost::system)
target_include_directories(shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)