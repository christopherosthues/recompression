cmake_minimum_required(VERSION 3.0.2)

option(RECOMPRESSION_ENABLE_TESTS "Enable building and running unit tests." ON)
option(RECOMPRESSION_ENABLE_EXPERIMENTS "Set to ON to automatically build all experiments." OFF)

project(recompression)
set(PROJECT_VENDOR "Christopher Osthues")
set(PROJECT_CONTACT "osthues.christopher@web.de")
set(PROJECT_URL "https://github.com/christopherosthues/recompression")
set(PROJECT_DESCRIPTION "Recompression")
#set(CMAKE_BUILD_TYPE "Release")

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
#cmake_policy(SET CMP0046 OLD)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

# includes build_library, build_executable and build_tests macros
include(Build)

set(SOURCES src/rlslp.cpp src/parallel_recompression.cpp src/util.cpp)
set(HEADERS include/defs.hpp include/rlslp.hpp include/parallel_recompression.hpp include/util.hpp)

build_library()

# building tests
set(BUILD_TESTS true)

add_subdirectory(test)

add_subdirectory(bench)
