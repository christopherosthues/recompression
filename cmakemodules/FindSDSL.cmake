include(FindPackageHandleStandardArgs)

set(SDSL_POSSIBLE_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/SDSL")

message(STATUS)
message(STATUS "Trying to locate package: SDSL")

# Temporarily disable REQUIRED if set
set(SDSL_FIND_REQUIRED_BACKUP ${SDSL_FIND_REQUIRED})
set(SDSL_FIND_REQUIRED 0)

# First dir to be searched for SDSL is in build dir
set(SDSL_ROOT_DIR ${SDSL_POSSIBLE_INSTALL_DIR})

find_path(SDSL_INCLUDE_DIR sdsl/config.hpp
        PATHS ${SDSL_ROOT_DIR}/include /usr/local/include /usr/include)

find_library(SDSL_LIBRARY sdsl
        PATHS ${SDSL_ROOT_DIR}/lib /usr/local/lib /usr/lib)
find_package_handle_standard_args(SDSL DEFAULT_MSG
        SDSL_LIBRARY SDSL_INCLUDE_DIR)

find_library(DIVSUFSORT_LIBRARY divsufsort
        PATHS ${SDSL_ROOT_DIR}/lib /usr/local/lib /usr/lib)
find_package_handle_standard_args(Divsufsort DEFAULT_MSG
        DIVSUFSORT_LIBRARY)

find_library(DIVSUFSORT64_LIBRARY divsufsort64
        PATHS ${SDSL_ROOT_DIR}/lib /usr/local/lib /usr/lib)
find_package_handle_standard_args(Divsufsort64 DEFAULT_MSG
        DIVSUFSORT64_LIBRARY)

# Restore REQUIRED
set(SDSL_FIND_REQUIRED ${SDSL_FIND_REQUIRED_BACKUP})

if (SDSL_FOUND AND DIVSUFSORT_FOUND AND DIVSUFSORT64_FOUND)
    set(SDSL_INCLUDE_DIRS ${SDSL_INCLUDE_DIR})
    set(SDSL_LIBRARIES ${SDSL_LIBRARY} ${DIVSUFSORT_LIBRARY} ${DIVSUFSORT64_LIBRARY})
    message(STATUS "Found all SDSL Libraries")
else ()
    message(STATUS "Could NOT locate SDSL on system -- preparing download")
    include(cmakemodules/DownloadSDSL.cmake)
endif ()
