cmake_minimum_required(VERSION 3.0.2)

option(RECOMPRESSION_ENABLE_TESTS "Set to ON to enable building and running unit tests." ON)
option(RECOMPRESSION_ENABLE_AUTO_RUN_TESTS "Set to ON to automatically run all tests after a successful build." ON)
option(RECOMPRESSION_ENABLE_BENCHMARKS "Set to ON to automatically build all benchmark experiments." ON)
option(RECOMPRESSION_GENERATE_DOC "Set ON to generate doxygen API reference in build/doc directory" OFF)

project(recompression)
set(PROJECT_VENDOR "Christopher Osthues")
set(PROJECT_CONTACT "osthues.christopher@web.de")
set(PROJECT_URL "https://github.com/christopherosthues/recompression")
set(PROJECT_DESCRIPTION "Recompression")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

message(STATUS "Building in '${CMAKE_BUILD_TYPE}' mode")

add_compile_options(-g)
add_compile_options(-W -Wall -Wextra -Wpedantic -Wno-deprecated)
add_compile_options(-O3 -ffast-math -ftree-vectorize -march=native)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    message(STATUS "Adding debug flags")
    add_definitions(-DDEBUG)
elseif(${CMAKE_BUILD_TYPE} MATCHES Bench)
    message(STATUS "Adding benchmark flags")
    add_definitions(-DBENCH)
    add_definitions(-DBENCH_RECOMP)
elseif(${CMAKE_BUILD_TYPE} MATCHES BenchRecompression)
    message(STATUS "Adding recompression benchmark flags")
    add_definitions(-DBENCH_RECOMP)
endif ()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

add_custom_target(external-downloads)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

# includes build_library, build_executable and build_tests macros
include(Build)

set(SOURCES src/defs.cpp src/rlslp.cpp src/recompression.cpp src/parallel_recompression.cpp src/util.cpp
        src/radix_sort.cpp src/sequential_recompression.cpp src/fast_recompression.cpp
        src/parallel_order_less_recompression.cpp src/full_parallel_recompression.cpp
        src/parallel_order_great_recompression.cpp src/lce_query.cpp)
set(HEADERS include/defs.hpp include/rlslp.hpp include/recompression.hpp include/parallel_recompression.hpp include/util.hpp
        include/radix_sort.hpp include/sequential_recompression.hpp include/fast_recompression.hpp
        include/parallel_order_less_recompression.hpp include/full_parallel_recompression.hpp
        include/parallel_order_great_recompression.hpp include/lce_query.hpp)

find_package(Git)
if (GIT_FOUND)
    message("git found at: ${GIT_EXECUTABLE}")
else ()
    message(WARNING "Could not find git. Cloning of submodules will not work.")
endif ()

build_library()

add_subdirectory(test)

add_subdirectory(bench)

add_subdirectory(docs)
