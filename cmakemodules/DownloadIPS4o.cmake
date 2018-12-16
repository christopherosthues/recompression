set(IPS4o_INCLUDE_DIRS ${IPS4o_INSTALL_DIR} ${IPS4o_INSTALL_DIR}/ips4o)
set(IPS4o_LIBRARIES ${IPS4o_INSTALL_DIR}/lib/libips4o.a)
set(IPS4o_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded IPS4o versions")
file(REMOVE_RECURSE ${IPS4o_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${IPS4o_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        IPS4o-download
        PREFIX ${IPS4o_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/SaschaWitt/ips4o.git
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND mkdir cmakemodules && cp "${CMAKE_SOURCE_DIR}/cmakemodules/Dependency.cmake" "cmakemodules/" &&
        cp "${CMAKE_SOURCE_DIR}/cmakemodules/ips4o/CMakeLists.txt" "." && mkdir build && cd build && cmake .. && make all
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND
        cd build && mkdir "${IPS4o_INSTALL_DIR}/include/" &&
        mv "../ips4o" "${IPS4o_INSTALL_DIR}/include/ips4o" && mv "../ips4o.hpp" "${IPS4o_INSTALL_DIR}/include" &&
        mkdir "${IPS4o_INSTALL_DIR}/lib" &&
        cp "libips4o.a" "${IPS4o_INSTALL_DIR}/lib/"
        INSTALL_DIR "${IPS4o_INSTALL_DIR}"
)
add_dependencies(external-downloads IPS4o-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(IPS4o DEFAULT_MSG IPS4o_INCLUDE_DIRS IPS4o_LIBRARIES)
