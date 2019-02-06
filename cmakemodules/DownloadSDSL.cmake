include(FindPackageHandleStandardArgs)

set(SDSL_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/SDSL-external")
set(SDSL_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/SDSL")

set(SDSL_INCLUDE_DIRS ${SDSL_INSTALL_DIR}/include)
# Set library names
set(SDSL_LIBRARIES
        ${SDSL_INSTALL_DIR}/lib/libdivsufsort.a
        ${SDSL_INSTALL_DIR}/lib/libdivsufsort64.a
        ${SDSL_INSTALL_DIR}/lib/libsdsl.a)
set(SDSL_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded SDSL versions")
file(REMOVE_RECURSE ${SDSL_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${SDSL_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        SDSL-download
        PREFIX ${SDSL_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/simongog/sdsl-lite.git
        GIT_TAG 7bbb71e8e13279ab45111bd135f4210545cd1c85
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        # Copy Make.helper to install dir. Necessary for FindGCSA2 to work
        BUILD_COMMAND ./install.sh "${SDSL_INSTALL_DIR}" && cp Make.helper "${SDSL_INCLUDE_DIRS}/../SDSL-Make.helper"
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND ""
)
add_dependencies(external-downloads SDSL-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDSL DEFAULT_MSG SDSL_INCLUDE_DIRS)

set(DIVSUFSORT_FOUND ${SDSL_FOUND})
set(DIVSUFSORT64_FOUND ${SDSL_FOUND})
