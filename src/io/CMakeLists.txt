add_library(io parquet.cpp parquet.hpp)
target_link_libraries(io PUBLIC arrow parquet PRIVATE spdlog)