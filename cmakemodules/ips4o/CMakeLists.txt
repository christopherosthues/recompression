cmake_minimum_required(VERSION 3.7)
project(ips4o CXX C)

add_compile_options(-g)
add_compile_options(-W -Wall -Wextra -Wpedantic -Wno-deprecated)
add_compile_options(-O3 -ffast-math -ftree-vectorize -march=native)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

add_custom_target(external-downloads)

include_directories(${CMAKE_SOURCE_DIR}/.)
include_directories(${CMAKE_SOURCE_DIR}/ips4o)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

include(cmakemodules/Dependency.cmake)

set(HEADERS ips4o/base_case.hpp
        ips4o/block_permutation.hpp
        ips4o/bucket_pointers.hpp
        ips4o/buffers.hpp
        ips4o/classifier.hpp
        ips4o/cleanup_margins.hpp
        ips4o/config.hpp
        ips4o/empty_block_movement.hpp
        ips4o/ips4o.hpp
        ips4o/ips4o_fwd.hpp
        ips4o/local_classification.hpp
        ips4o/memory.hpp
        ips4o/parallel.hpp
        ips4o/partitioning.hpp
        ips4o/sampling.hpp
        ips4o/sequential.hpp
        ips4o/synchronization.hpp
        ips4o/thread_pool.hpp
        ips4o/utils.hpp
        ips4o.hpp)

add_library(${PROJECT_NAME} STATIC ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
add_dependency(${PROJECT_NAME} "OpenMP" REQUIRED)
