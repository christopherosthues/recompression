include(FindPackageHandleStandardArgs)

set(Kahip_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/Kahip-external")
set(Kahip_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/Kahip")

message(STATUS)
message(STATUS "Trying to locate package: Kahip")

# Temporarily disable REQUIRED if set
set(Kahip_FIND_REQUIRED_BACKUP ${Kahip_FIND_REQUIRED})
set(Kahip_FIND_REQUIRED 0)

set(Kahip_ROOT_DIR ${Kahip_INSTALL_DIR})

find_path(Kahip_INCLUDE_DIR kaHIP_interface.h parhip_interface.h
        PATHS ${Kahip_ROOT_DIR} ${Kahip_ROOT_DIR}/include /usr/local/include /usr/include)

find_library(Kahip_LIBRARY kahip PATHS ${Kahip_ROOT_DIR}/lib /usr/local/lib /usr/lib)
#
#find_package_handle_standard_args(IPS4o DEFAULT_MSG Kahip_INCLUDE_DIR Kahip_LIBRARY)
find_package_handle_standard_args(Kahip DEFAULT_MSG Kahip_INCLUDE_DIR)

# Restore REQUIRED
set(Kahip_FIND_REQUIRED ${Kahip_FIND_REQUIRED_BACKUP})

if (Kahip_FOUND)
    set(Kahip_INCLUDE_DIRS ${Kahip_INCLUDE_DIR})
    #    set(Kahip_LIBRARIES ${Kahip_LIBRARY})
    #    message(STATUS "Found ${Kahip_LIBRARIES}")
    message(STATUS "Found ${Kahip_INCLUDE_DIRS}")
else ()
    message(STATUS "Could NOT locate Kahip on system -- preparing download")
    include(cmakemodules/DownloadKahip.cmake)
endif ()
