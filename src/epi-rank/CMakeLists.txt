add_executable(epi-rank epi-rank.cpp SafegraphCountyMapper.cpp SafegraphCountyMapper.hpp SafegraphCountyTiler.cpp SafegraphCountyTiler.hpp EpiConfig.cpp BlazeMatricies.cpp BlazeMatricies.hpp)
target_link_libraries(epi-rank PRIVATE MCPP::map-tile shared io yaml-cpp::yaml-cpp MCPP::components mcpp-malloc)
target_compile_options(epi-rank PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(epi-rank PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
add_sanitizers(epi-rank)