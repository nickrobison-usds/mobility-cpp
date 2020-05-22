add_executable(tests simple.cpp)
target_link_libraries(tests Catch2::Catch2)

add_test(tests tests)