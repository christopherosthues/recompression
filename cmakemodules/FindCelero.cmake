include(FindPackageHandleStandardArgs)

set(Celero_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/Celero-external")
set(Celero_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/Celero")

message(STATUS)
message(STATUS "Trying to locate package: Celero")

# Temporarily disable REQUIRED if set
set(Celero_FIND_REQUIRED_BACKUP ${Celero_FIND_REQUIRED})
set(Celero_FIND_REQUIRED 0)

set(Celero_ROOT_DIR ${Celero_INSTALL_DIR})

find_path(Celero_INCLUDE_DIR celero/Celero.h
        PATHS ${Celero_ROOT_DIR}/include /usr/local/include /usr/include)

find_library(Celero_LIBRARY celero
        PATHS ${Celero_ROOT_DIR}/lib /usr/local/lib /usr/lib)

find_package_handle_standard_args(Celero DEFAULT_MSG Celero_INCLUDE_DIR Celero_LIBRARY)

# Restore REQUIRED
set(Celero_FIND_REQUIRED ${Celero_FIND_REQUIRED_BACKUP})

if (Celero_FOUND)
    set(Celero_INCLUDE_DIRS ${Celero_INCLUDE_DIR})
    set(Celero_LIBRARIES ${Celero_LIBRARY})
    message(STATUS "Found ${Celero_LIBRARIES}")
else ()
    message(STATUS "Could NOT locate Celero on system -- preparing download")
    include(cmakemodules/DownloadCelero.cmake)
endif ()
