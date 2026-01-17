cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

## Use -std=c++17 as default.
set(CMAKE_CXX_STANDARD 17)
## Disable C++ extensions
set(CMAKE_CXX_EXTENSIONS OFF)
## Require full C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the helib install location
set(helib_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../helib_install/share/cmake/helib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(encoding_switching_benchmarks
        LANGUAGES CXX)

find_package(helib REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src)

# Build workload benchmark
add_executable(workload
    src/workload.cpp
    src/bridge.cpp
    src/tools.cpp
    src/Ctxt_ext.cpp)
target_link_libraries(workload helib)

# Build decision tree benchmark
add_executable(decision_tree
    src/decision_tree.cpp
    src/bridge.cpp
    src/tools.cpp
    src/Ctxt_ext.cpp)
target_link_libraries(decision_tree helib)

# Build sorting benchmark
add_executable(sorting
    src/sorting.cpp
    src/bridge.cpp
    src/tools.cpp
    src/Ctxt_ext.cpp)
target_link_libraries(sorting helib)

# Build Floyd-Warshall benchmark
add_executable(floyd_warshall
    src/floyd_warshall.cpp
    src/bridge.cpp
    src/tools.cpp
    src/Ctxt_ext.cpp)
target_link_libraries(floyd_warshall helib)

# Build database aggregation benchmark
add_executable(database_aggregation
    src/database_aggregation.cpp
    src/bridge.cpp
    src/tools.cpp
    src/Ctxt_ext.cpp)
target_link_libraries(database_aggregation helib)

# Build quick test for fast smoke testing (2-3 minutes)
# Tests 6-bit workload only
add_executable(quick_test
    src/quick_test.cpp
    src/bridge.cpp
    src/tools.cpp
    src/Ctxt_ext.cpp)
target_link_libraries(quick_test helib)

message(STATUS "Build configuration complete. Use 'make' to build the benchmarks.")
message(STATUS "Executables will be in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
