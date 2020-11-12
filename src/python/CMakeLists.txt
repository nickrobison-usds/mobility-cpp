add_library(mcpp-python src/PythonInterpreter.cpp include/python/PythonInterpreter.hpp include/python/PythonTiler.hpp include/python/PandasEngine.hpp include/python/helpers.hpp)
target_link_libraries(mcpp-python PUBLIC pybind11::embed MCPP::map-tile)
target_include_directories(mcpp-python PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_library(MCPP::python ALIAS mcpp-python)

add_executable(python-tests test/interpreter_test.cpp test/pandas_test.cpp test/person.hpp test/person_diff.hpp)
target_link_libraries(python-tests Catch2::Catch2 MCPP::python)
add_test(python-tests python-tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/python/test)
