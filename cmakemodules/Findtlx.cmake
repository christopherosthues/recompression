include(FindPackageHandleStandardArgs)

set(tlx_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/tlx-external")
set(tlx_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/tlx")

message(STATUS)
message(STATUS "Trying to locate package: tlx")

# Temporarily disable REQUIRED if set
set(tlx_FIND_REQUIRED_BACKUP ${tlx_FIND_REQUIRED})
set(tlx_FIND_REQUIRED 0)

set(tlx_ROOT_DIR ${tlx_INSTALL_DIR})

find_path(tlx_INCLUDE_DIR tlx PATHS ${tlx_ROOT_DIR} ${tlx_ROOT_DIR}/include /usr/local/include /usr/include)

find_library(tlx_LIBRARY tlx PATHS ${tlx_ROOT_DIR}/lib /usr/local/lib /usr/lib)

find_package_handle_standard_args(tlx DEFAULT_MSG tlx_INCLUDE_DIR tlx_LIBRARY)
#find_package_handle_standard_args(tlx DEFAULT_MSG tlx_INCLUDE_DIR)

# Restore REQUIRED
set(tlx_FIND_REQUIRED ${tlx_FIND_REQUIRED_BACKUP})

if (tlx_FOUND)
    set(tlx_INCLUDE_DIRS ${tlx_INCLUDE_DIR})
    set(tlx_LIBRARIES ${tlx_LIBRARY})
    message(STATUS "Found ${tlx_LIBRARIES}")
    message(STATUS "Found ${tlx_INCLUDE_DIRS}")
else ()
    message(STATUS "Could NOT locate tlx on system -- preparing download")
    include(cmakemodules/Downloadtlx.cmake)
endif ()
