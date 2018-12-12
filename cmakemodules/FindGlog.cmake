
# - Try to find Glog
#
# The following variables are optionally searched for defaults
#  GLOG_ROOT_DIR:            Base directory where all GLOG components are found
#
# The following are set after configuration is done:
#  GLOG_FOUND
#  GLOG_INCLUDE_DIRS
#  GLOG_LIBRARIES
#  GLOG_LIBRARYRARY_DIRS

include(FindPackageHandleStandardArgs)

set(Glog_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/Glog-external")
set(Glog_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/Glog")

set(Glog_ROOT_DIR ${Glog_INSTALL_DIR})

if (WIN32)
    find_path(Glog_INCLUDE_DIR glog/logging.h PATHS ${Glog_ROOT_DIR}/src/windows)
else ()
    find_path(Glog_INCLUDE_DIR glog/logging.h PATHS ${Glog_ROOT_DIR})
endif ()

if (MSVC)
    find_library(Glog_LIBRARY_RELEASE libglog_static PATHS ${Glog_ROOT_DIR} PATH_SUFFIXES Release)

    find_library(Glog_LIBRARY_DEBUG libglog_static PATHS ${Glog_ROOT_DIR} PATH_SUFFIXES Debug)

    set(Glog_LIBRARY optimized ${Glog_LIBRARY_RELEASE} debug ${Glog_LIBRARY_DEBUG})
else ()
    find_library(Glog_LIBRARY glog PATHS ${Glog_ROOT_DIR} PATH_SUFFIXES lib lib64)
endif ()

find_package_handle_standard_args(Glog DEFAULT_MSG Glog_INCLUDE_DIR Glog_LIBRARY)

if (Glog_FOUND)
    set(Glog_INCLUDE_DIRS ${Glog_INCLUDE_DIR})
    set(Glog_LIBRARIES ${Glog_LIBRARY})
else ()
    message(STATUS "Could NOT locate Glog on system -- preparing download")
    include(DownloadGlog.cmake)
endif ()