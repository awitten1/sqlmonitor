cmake_minimum_required(VERSION 3.24)

project(sqlmonitor LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp procfs.cpp ddl.cpp)
find_package(fmt REQUIRED)
find_package(gflags REQUIRED)
add_subdirectory(duckdb)
target_link_libraries(main PRIVATE duckdb_static gflags fmt::fmt)