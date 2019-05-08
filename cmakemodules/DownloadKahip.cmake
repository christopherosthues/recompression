include(FindPackageHandleStandardArgs)

set(Kahip_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp/Kahip-external")
set(Kahip_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/Kahip")

set(Kahip_INCLUDE_DIRS ${Kahip_INSTALL_DIR}/include)
# Set library names
set(Kahip_LIBRARIES
        ${Kahip_INSTALL_DIR}/lib/libkahip.a
        ${Kahip_INSTALL_DIR}/lib/libparhip.a)
set(Kahip_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded Kahip versions")
file(REMOVE_RECURSE ${Kahip_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${Kahip_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        Kahip-download
        PREFIX ${Kahip_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/schulzchristian/KaHIP.git
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        # Copy Make.helper to install dir. Necessary for FindGCSA2 to work
        BUILD_COMMAND ./compile_withcmake.sh
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND mkdir -p "${Kahip_INSTALL_DIR}/include/" && mkdir -p "${Kahip_INSTALL_DIR}/lib/" &&
        cp deploy/libkahip.a deploy/libparhip.a "${Kahip_INSTALL_DIR}/lib/" && cp deploy/kaHIP_interface.h deploy/parhip_interface.h "${Kahip_INSTALL_DIR}/include/"
)
add_dependencies(external-downloads Kahip-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Kahip DEFAULT_MSG Kahip_INCLUDE_DIRS)

