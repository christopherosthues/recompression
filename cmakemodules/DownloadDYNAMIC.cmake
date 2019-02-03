#set(Dynamic_INCLUDE_DIRS ${Dynamic_INSTALL_DIR} ${Dynamic_INSTALL_DIR}/include)
#set(Dynamic_LIBRARIES ${Dynamic_INSTALL_DIR}/lib/libdynamic.a)
#set(Dynamic_EXTERNAL TRUE)
#
#message(STATUS "Removing remains of previously downloaded Dynamic versions")
#file(REMOVE_RECURSE ${Dynamic_DOWNLOAD_DIR})
#file(REMOVE_RECURSE ${Dynamic_INSTALL_DIR})
#
#include(ExternalProject)
#ExternalProject_Add(
#        Dynamic-download
#        PREFIX ${Dynamic_DOWNLOAD_DIR}
#        GIT_REPOSITORY https://github.com/xxsds/DYNAMIC.git
#        PATCH_COMMAND ""
#        UPDATE_COMMAND ""
#        CONFIGURE_COMMAND ""
#        BUILD_COMMAND mkdir build && cd build && cmake .. && make
#        BUILD_IN_SOURCE TRUE
#        INSTALL_COMMAND
##        cd build && mkdir "${Dynamic_INSTALL_DIR}/include/" &&
##        mv "../ips4o" "${Dynamic_INSTALL_DIR}/include/ips4o" && mv "../ips4o.hpp" "${Dynamic_INSTALL_DIR}/include" &&
##        mkdir "${Dynamic_INSTALL_DIR}/lib" &&
##        cp "libips4o.a" "${Dynamic_INSTALL_DIR}/lib/"
#        INSTALL_DIR "${Dynamic_INSTALL_DIR}"
#)
#add_dependencies(external-downloads Dynamic-download)
#
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(Dynamic DEFAULT_MSG Dynamic_INCLUDE_DIRS Dynamic_LIBRARIES)


set(DYNAMIC_INCLUDE_DIRS ${DYNAMIC_INSTALL_DIR}/include)
set(DYNAMIC_INCLUDE_DIRS ${DYNAMIC_INCLUDE_DIRS} ${DYNAMIC_INSTALL_DIR}/include/algorithms ${DYNAMIC_INSTALL_DIR}/include/internal)
set(DYNAMIC_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded DYNAMIC versions")
file(REMOVE_RECURSE ${DYNAMIC_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${DYNAMIC_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        DYNAMIC-download
        PREFIX ${DYNAMIC_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/xxsds/DYNAMIC.git
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND cp -r include "${DYNAMIC_INSTALL_DIR}/include"
        INSTALL_DIR "${DYNAMIC_INSTALL_DIR}"
)

add_dependencies(external-downloads DYNAMIC-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(DYNAMIC DEFAULT_MSG DYNAMIC_INSTALL_DIR)