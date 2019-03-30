set(tlx_INCLUDE_DIRS ${tlx_INSTALL_DIR} ${tlx_INSTALL_DIR}/include)
set(tlx_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded tlx versions")
file(REMOVE_RECURSE ${tlx_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${tlx_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        tlx-download
        PREFIX ${tlx_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/tlx/tlx.git
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND mkdir build && cd build && cmake .. && make all
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND mkdir -p "${tlx_INSTALL_DIR}/include/" && mkdir -p "${tlx_INSTALL_DIR}/lib/" &&
            cp build/tlx/libtlx.a "${tlx_INSTALL_DIR}/lib/" &&
            find tlx -type f -name "*.hpp" -exec cp --parents -t "${tlx_INSTALL_DIR}/include/" "{}" +;
        INSTALL_DIR "${tlx_INSTALL_DIR}"
)
add_dependencies(external-downloads tlx-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(tlx DEFAULT_MSG tlx_INCLUDE_DIRS)