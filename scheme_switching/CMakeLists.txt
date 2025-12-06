cmake_minimum_required(VERSION 3.16.3)


project(demo CXX)
set(CMAKE_CXX_STANDARD 17)
option(BUILD_STATIC "Set to ON to include static versions of the library" OFF)

set(OpenFHE_INCLUDE "/home/jiaq/Research/Code/Universal/openfhelibs/include")
set(OpenFHE_LIBDIR "/home/jiaq/Research/Code/Universal/openfhelibs/lib")

set(BENCHMARK_INCLUDE "/home/jiaq/Research/Code/Universal/benchmarklibs/include")
set(BENCHMARK_LIBDIR "/home/jiaq/Research/Code/Universal/benchmarklibs/lib")

find_package(OpenFHE CONFIG REQUIRED)
if(OpenFHE_FOUND)
    message(STATUS "FOUND PACKAGE OpenFHE")
    message(STATUS "OpenFHE Version: ${BASE_OPENFHE_VERSION}")
    message(STATUS "OpenFHE installed as shared libraries: ${OpenFHE_SHARED}")
    message(STATUS "OpenFHE include files location: ${OpenFHE_INCLUDE}")
    message(STATUS "OpenFHE lib files location: ${OpenFHE_LIBDIR}")
    message(STATUS "OpenFHE Native Backend size: ${OpenFHE_NATIVE_SIZE}")
else()
    message(FATAL_ERROR "PACKAGE OpenFHE NOT FOUND")
endif()

set(CMAKE_CXX_FLAGS ${OpenFHE_CXX_FLAGS})

include_directories(${OPENMP_INCLUDES})
include_directories(${OpenFHE_INCLUDE})
include_directories(${OpenFHE_INCLUDE}/third-party/include)
include_directories(${OpenFHE_INCLUDE}/core)
include_directories(${OpenFHE_INCLUDE}/pke)
include_directories(${OpenFHE_INCLUDE}/binfhe)
include_directories(${BENCHMARK_INCLUDE})
### add directories for other OpenFHE modules as needed for your project

link_directories(${OpenFHE_LIBDIR})
link_directories(${BENCHMARK_LIBDIR})
link_directories(${OPENMP_LIBRARIES})
if(BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "${OpenFHE_EXE_LINKER_FLAGS} -static")
    link_libraries(${OpenFHE_STATIC_LIBRARIES})
else()
    set(CMAKE_EXE_LINKER_FLAGS ${OpenFHE_EXE_LINKER_FLAGS})
    link_libraries(${OpenFHE_SHARED_LIBRARIES})
endif()

### ADD YOUR EXECUTABLE(s) HERE
### add_executable(EXECUTABLE-NAME SOURCES)
###
### EXAMPLE:
### add_executable(test demo-simple-example.cpp)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Scheme Switching Benchmark Suite
add_executable(workload 
    src/workload.cpp
    src/utils.cpp
)

# Link with benchmark library
target_link_libraries(workload benchmark)

add_executable(decision_tree 
    src/decision_tree.cpp
    src/utils.cpp
)

add_executable(sorting 
    src/sorting.cpp
    src/utils.cpp
)

add_executable(floyd_warshall 
    src/floyd_warshall.cpp
    src/utils.cpp
)

add_executable(database_aggregation
    src/database_aggregation.cpp
    src/utils.cpp
)

add_executable(test_basic
    src/test_basic.cpp
    src/utils.cpp
)

add_executable(test_decision_tree_small
    src/test_decision_tree_small.cpp
    src/utils.cpp
)

# # Scheme Switching Benchmark Suite
# add_executable(scheme-switching-benchmark-test
#     src/main_test.cpp
# )

# # Link with benchmark library
# target_link_libraries(scheme-switching-benchmark-test benchmark)