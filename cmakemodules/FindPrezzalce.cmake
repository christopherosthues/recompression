include(FindPackageHandleStandardArgs)

set(Prezzalce_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/Prezzalce-external")
set(Prezzalce_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/prezzalce")

message(STATUS)
message(STATUS "Trying to locate package: prezzalce")

# Temporarily disable REQUIRED if set
set(Prezzalce_FIND_REQUIRED_BACKUP ${Prezzalce_FIND_REQUIRED})
set(Prezzalce_FIND_REQUIRED 0)

set(Prezzalce_ROOT_DIR ${Prezzalce_INSTALL_DIR})

find_path(Prezzalce_INCLUDE_DIR lce.hpp PATHS ${Prezzalce_ROOT_DIR} ${Prezzalce_ROOT_DIR}/include /usr/local/include /usr/include)

find_package_handle_standard_args(Prezzalce DEFAULT_MSG Prezzalce_INCLUDE_DIR)

# Restore REQUIRED
set(Prezzalce_FIND_REQUIRED ${Prezzalce_FIND_REQUIRED_BACKUP})

if (Prezzalce_FOUND)
    set(Prezzalce_INCLUDE_DIRS ${Prezzalce_INCLUDE_DIR})
    #    set(Prezzalce_LIBRARIES ${Prezzalce_LIBRARY})
    #    message(STATUS "Found ${Prezzalce_LIBRARIES}")
    message(STATUS "Found ${Prezzalce_INCLUDE_DIRS}")
else ()
    message(STATUS "Could NOT locate prezzalce on system -- preparing download")
    message(STATUS "Modules: ${CMAKE_MODULE_PATH}")
    include(cmakemodules/DownloadPrezzalce.cmake)
endif ()
