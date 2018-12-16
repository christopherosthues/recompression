set(Celero_INCLUDE_DIRS ${Celero_INSTALL_DIR} ${Celero_INSTALL_DIR}/include)
set(Celero_LIBRARIES ${Celero_INSTALL_DIR}/lib/libcelero.so)
set(Celero_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded Celero versions")
file(REMOVE_RECURSE ${Celero_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${Celero_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        Celero-download
        PREFIX ${Celero_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/DigitalInBlue/Celero.git
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND mkdir build && cd build && cmake .. && make
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND
        cd build && mv "../include" "${Celero_INSTALL_DIR}/include" &&
        mkdir "${Celero_INSTALL_DIR}/lib" &&
        cp "libcelero.so" "${Celero_INSTALL_DIR}/lib/"
        INSTALL_DIR "${Celero_INSTALL_DIR}"
)
add_dependencies(external-downloads Celero-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Celero DEFAULT_MSG Celero_INCLUDE_DIRS Celero_LIBRARIES)
