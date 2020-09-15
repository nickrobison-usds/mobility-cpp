# It appears that Spectra doesn't seem to work as a submodule: https://github.com/yixuan/spectra/issues/95. So we'll just create a manual target
add_library(spectra INTERFACE)
target_include_directories(spectra INTERFACE ${CMAKE_SOURCE_DIR}/extern/spectra/include)

add_executable(epi-rank epi-rank.cpp SafegraphCountyMapper.cpp SafegraphCountyMapper.hpp SafegraphCountyTiler.cpp SafegraphCountyTiler.hpp EpiConfig.cpp EigenMatricies.cpp EigenMatricies.hpp BlazeMatricies.cpp BlazeMatricies.hpp)
target_link_libraries(epi-rank PRIVATE MCPP::map-tile shared yaml-cpp::yaml-cpp MCPP::components Eigen3::Eigen spectra)
target_compile_options(epi-rank PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(epi-rank PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
add_sanitizers(epi-rank)

if (${DEFAULT_MALLOC} STREQUAL "tcmalloc")
    message("Linking with TCMalloc")
    target_link_libraries(epi-rank PRIVATE ${TCMALLOC_LIBRARY})
endif ()