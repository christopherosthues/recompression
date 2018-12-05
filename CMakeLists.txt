cmake_minimum_required(VERSION 3.10)

project(recompression)
set(PROJECT_VENDOR "Christopher Osthues")
set(PROJECT_CONTACT "osthues.christopher@web.de")
set(PROJECT_URL "https://github.com/christopherosthues/recompression")
set(PROJECT_DESCRIPTION "Recompression")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

message(STATUS "Building in '${CMAKE_BUILD_TYPE}' mode")

add_compile_options(-g)
add_compile_options(-W -Wall -Wextra -Wpedantic -Wno-deprecated)
add_compile_options(-O3 -ffast-math -ftree-vectorize -march=native)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    message(STATUS "Adding debug flags")
    add_definitions(-DDEBUG)
endif ()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")

add_custom_target(external-downloads)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

# includes build_library, build_executable and build_tests macros
include(Build)

set(SOURCES src/rlslp.cpp src/parallel_recompression.cpp)
set(HEADERS include/rlslp.hpp include/parallel_recompression.hpp)

build_library()

# building tests
set(BUILD_TESTS true)

build_tests("bcomp")
build_tests("pcomp")
build_tests("multiset")
build_tests("partition")
build_tests("recomp")
build_tests("radix_sort")
