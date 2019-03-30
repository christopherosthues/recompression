include(FindPackageHandleStandardArgs)

set(DYNAMIC_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/DYNAMIC-external")
set(DYNAMIC_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/DYNAMIC")

message(STATUS)
message(STATUS "Trying to locate package: DYNAMIC")

# Temporarily disable REQUIRED if set
set(DYNAMIC_FIND_REQUIRED_BACKUP ${DYNAMIC_FIND_REQUIRED})
set(DYNAMIC_FIND_REQUIRED 0)

# First dir to be searched for DYNAMIC is DYNAMIC_INSTALL_DIR in build dir
set(DYNAMIC_ROOT_DIR ${DYNAMIC_INSTALL_DIR})

find_path(DYNAMIC_INCLUDE_PATH dynamic.hpp
        PATHS ${DYNAMIC_ROOT_DIR}/include /usr/local/include /usr/include)

set(DYNAMIC_INCLUDE_DIRS ${DYNAMIC_INCLUDE_PATH} ${DYNAMIC_INCLUDE_PATH}/algorithms ${DYNAMIC_INCLUDE_PATH}/internal)
find_package_handle_standard_args(DYNAMIC DEFAULT_MSG DYNAMIC_INCLUDE_PATH)

# Restore REQUIRED
set(DYNAMIC_FIND_REQUIRED ${DYNAMIC_FIND_REQUIRED_BACKUP})

if (DYNAMIC_FOUND)
#    set(DYNAMIC_INCLUDE_DIRS ${DYNAMIC_INCLUDE_DIR})
#    set(DYNAMIC_LIBRARIES ${DYNAMIC_LIBRARY})
    message(STATUS "Found ${DYNAMIC_INCLUDE_DIRS}")
    message(STATUS "Found ${DYNAMIC_LIBRARIES}")
else ()
    message(STATUS "Could NOT locate DYNAMIC on system -- preparing download")
    include(cmakemodules/DownloadDYNAMIC.cmake)
endif ()
