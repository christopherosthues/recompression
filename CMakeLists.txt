cmake_minimum_required(VERSION 3.0.2)

option(RECOMPRESSION_ENABLE_TESTS "Set to ON to enable building and running unit tests." OFF)
option(RECOMPRESSION_ENABLE_AUTO_RUN_TESTS "Set to ON to automatically run all tests after a successful build." ON)
option(RECOMPRESSION_ENABLE_BENCHMARKS "Set to ON to automatically build all benchmark experiments." ON)
option(RECOMPRESSION_GENERATE_DOC "Set ON to generate doxygen API reference in build/doc directory" ON)
option(RECOMPRESSION_ENABLE_MALLOC_COUNT "Set ON to enable memory measurement." OFF)

project(recompression)
set(PROJECT_VENDOR "Christopher Osthues")
set(PROJECT_CONTACT "")
set(PROJECT_URL "https://github.com/christopherosthues/recompression")
set(PROJECT_DESCRIPTION "Recompression")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

message(STATUS "Building in '${CMAKE_BUILD_TYPE}' mode")

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_COMPILER mpicxx)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fpermissive")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -g -std=c++14 -fpermissive -W -Wall -Wextra -Wpedantic -Wno-deprecated")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -ffast-math -ftree-vectorize -march=native")
#set(CMAKE_CXX_FLAGS_BENCH "${CMAKE_CXX_FLAGS_RELEASE} -DBENCH -DBENCH_RECOMP")
#set(CMAKE_CXX_FLAGS_BENCHRECOMPRESSION "${CMAKE_CXX_FLAGS_RELEASE} -DBENCH_RECOMP")

add_compile_options(-g)
add_compile_options(-W -Wall -Wextra -Wpedantic -Wno-deprecated)
add_compile_options(-O3 -fopenmp -ffast-math -funroll-loops -ftree-vectorize -march=native)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    message(STATUS "Adding debug flags")
    add_definitions(-DDEBUG)
elseif(${CMAKE_BUILD_TYPE} MATCHES Bench)
    message(STATUS "Adding benchmark flags")
    add_definitions(-DBENCH)
    add_definitions(-DBENCH_RECOMP)
elseif(${CMAKE_BUILD_TYPE} MATCHES BenchRecompression)
    message(STATUS "Adding recompression benchmark flags")
    add_definitions(-DBENCH_RECOMP)
elseif(${CMAKE_BUILD_TYPE} MATCHES BenchMemory)
    message(STATUS "Adding memory benchmark flags")
elseif(${CMAKE_BUILD_TYPE} MATCHES BenchSingleExtract)
    message(STATUS "Adding single extract flags")
    add_definitions(-DBENCH_SINGLE_EXTRACT)
endif ()

#add_definitions(-DGRAPH_STATS)

add_custom_target(external-downloads)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

# includes build_library, build_executable and build_tests macros
include(Build)

include(file_walker.cmake)

find_package(Git)
if (GIT_FOUND)
    message("git found at: ${GIT_EXECUTABLE}")
else ()
    message(WARNING "Could not find git. Cloning of submodules will not work.")
endif ()

build_library()

#add_subdirectory(test)

add_subdirectory(bench)

add_subdirectory(docs)
