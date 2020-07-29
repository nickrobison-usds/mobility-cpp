add_library(shared
        src/HostnameLogger.cpp include/shared/HostnameLogger.hpp
        src/DateUtils.cpp include/shared/DateUtils.hpp
        src/DirectoryUtils.cpp include/shared/DirectoryUtils.hpp
        include/shared/serializers.hpp src/serializers.cpp
        include/shared/data.hpp)

target_link_libraries(shared PUBLIC spdlog date HPX::hpx PRIVATE Boost::system)
target_include_directories(shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)