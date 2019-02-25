set(Prezzalce_INCLUDE_DIRS ${Prezzalce_INSTALL_DIR} ${Prezzalce_INSTALL_DIR}/include)
set(Prezzalce_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded prezzalce versions")
file(REMOVE_RECURSE ${Prezzalce_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${Prezzalce_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        Prezzalce-download
        PREFIX ${Prezzalce_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/al-xyz/prezza-lce
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND mkdir -p "${Prezzalce_INSTALL_DIR}/include/prezzalce" && cp -t "${Prezzalce_INSTALL_DIR}/include/prezzalce/" src/lce.hpp src/util.hpp
        INSTALL_DIR "${Prezzalce_INSTALL_DIR}"
)
add_dependencies(external-downloads Prezzalce-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(prezzalce DEFAULT_MSG Prezzalce_INCLUDE_DIRS)