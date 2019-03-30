include(FindPackageHandleStandardArgs)

set(sparsepp_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/sparsepp-external")
set(sparsepp_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/DYNAMIC")

message(STATUS)
message(STATUS "Trying to locate package: sparsepp")

# Temporarily disable REQUIRED if set
set(sparsepp_FIND_REQUIRED_BACKUP ${sparsepp_FIND_REQUIRED})
set(sparsepp_FIND_REQUIRED 0)

set(sparsepp_ROOT_DIR ${sparsepp_INSTALL_DIR})

find_path(sparsepp_INCLUDE_DIR internal/spp.h
        PATHS ${sparsepp_ROOT_DIR} ${sparsepp_ROOT_DIR}/include /usr/local/include /usr/include)

find_package_handle_standard_args(sparsepp DEFAULT_MSG sparsepp_INCLUDE_DIR)

# Restore REQUIRED
set(sparsepp_FIND_REQUIRED ${sparsepp_FIND_REQUIRED_BACKUP})

if (sparsepp_FOUND)
    message(STATUS "Found ${sparsepp_INCLUDE_DIRS}")
else ()
    message(STATUS "Could NOT locate sparsepp on system -- preparing download")
    include(cmakemodules/Downloadsparsepp.cmake)
endif ()
