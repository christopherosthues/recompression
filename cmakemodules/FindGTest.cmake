# - Find GTest
#
# This module tries to locate GTest on the system. If it is not found, it will
# automatically be downloaded and built from GitHub.
#
# The following variables will be set:
#
# GTEST_FOUND and GTest_FOUND
# GTest_INCLUDE_DIRS
# GTest_LIBRARIES (use this, if your tests come with a main-method implementation)
# GTest_Main_LIBRARIES (use this, if your tests do not come with a main-method implementation)
# GTest_INSTALL_DIR and GTest_DOWNLOAD_DIR (if download is necessary)

include(FindPackageHandleStandardArgs)

set(GTest_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/GTest-external")
set(GTest_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/GTest")

message(STATUS)
message(STATUS "Trying to locate package: GTest")

# Temporarily disable REQUIRED if set
set(GTest_FIND_REQUIRED_BACKUP ${GTest_FIND_REQUIRED})
set(GTest_FIND_REQUIRED 0)

# First dir to be searched for GTest is in build dir
set(GTest_ROOT_DIR ${GTest_INSTALL_DIR})

find_path(GTest_INCLUDE_DIRS gtest/gtest.h
        PATHS ${GTest_ROOT_DIR}/include /usr/local/include /usr/include)

find_library(GTest_LIBRARIES gtest
        PATHS ${GTest_ROOT_DIR}/lib /usr/local/lib /usr/lib)
find_package_handle_standard_args(GTest DEFAULT_MSG GTest_LIBRARIES GTest_INCLUDE_DIRS)

find_library(GTest_Main_LIBRARIES gtest_main
        PATHS ${GTest_ROOT_DIR}/lib /usr/local/lib /usr/lib)
find_package_handle_standard_args(GTest_Main DEFAULT_MSG GTest_Main_LIBRARIES)

# Restore REQUIRED
set(GTest_FIND_REQUIRED ${GTest_FIND_REQUIRED_BACKUP})

if (GTEST_FOUND AND GTEST_MAIN_FOUND)
    set(GTest_Main_LIBRARIES ${GTest_Main_LIBRARIES} ${GTest_LIBRARIES})
    message(STATUS "Found all GTest Libraries")
else ()
    message(STATUS "Could NOT locate GTest on system -- preparing download")
    include(cmakemodules/DownloadGTest.cmake)
endif ()
