add_executable(mcpp-tests simple.cpp)
target_link_libraries(mcpp-tests Catch2::Catch2)

add_test(mcpp-tests mcpp-tests)