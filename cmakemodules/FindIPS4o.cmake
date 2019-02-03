include(FindPackageHandleStandardArgs)

set(IPS4o_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/IPS4o-external")
set(IPS4o_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/IPS4o")

message(STATUS)
message(STATUS "Trying to locate package: IPS4o")

# Temporarily disable REQUIRED if set
set(IPS4o_FIND_REQUIRED_BACKUP ${IPS4o_FIND_REQUIRED})
set(IPS4o_FIND_REQUIRED 0)

set(IPS4o_ROOT_DIR ${IPS4o_INSTALL_DIR})

find_path(IPS4o_INCLUDE_DIR ips4o.hpp
        PATHS ${IPS4o_ROOT_DIR} ${IPS4o_ROOT_DIR}/include /usr/local/include /usr/include)

#find_library(IPS4o_LIBRARY ips4o
#        PATHS ${IPS4o_ROOT_DIR}/lib /usr/local/lib /usr/lib)
#
#find_package_handle_standard_args(IPS4o DEFAULT_MSG IPS4o_INCLUDE_DIR IPS4o_LIBRARY)
find_package_handle_standard_args(IPS4o DEFAULT_MSG IPS4o_INCLUDE_DIR)

# Restore REQUIRED
set(IPS4o_FIND_REQUIRED ${IPS4o_FIND_REQUIRED_BACKUP})

if (IPS4o_FOUND)
#    set(IPS4o_INCLUDE_DIRS ${IPS4o_INCLUDE_DIR})
#    set(IPS4o_LIBRARIES ${IPS4o_LIBRARY})
#    message(STATUS "Found ${IPS4o_LIBRARIES}")
    message(STATUS "Found ${IPS4o_INCLUDE_DIRS}")
else ()
    message(STATUS "Could NOT locate IPS4o on system -- preparing download")
    include(cmakemodules/DownloadIPS4o.cmake)
endif ()
