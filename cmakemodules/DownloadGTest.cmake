set(GTest_INCLUDE_DIRS ${GTest_INSTALL_DIR}/include)
set(GTest_LIBRARIES ${GTest_INSTALL_DIR}/lib/libgtest.a)
set(GTest_Main_LIBRARIES ${GTest_INSTALL_DIR}/lib/libgtest_main.a ${GTest_LIBRARIES})

message(STATUS "Removing remains of previously downloaded GTest versions")
file(REMOVE_RECURSE ${GTest_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${GTest_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        GTest-download
        PREFIX ${GTest_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/google/googletest.git
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND cd googletest && mkdir build && cd build && cmake .. && make all
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND
        cd googletest/build &&
        mv ../include "${GTest_INSTALL_DIR}/include" &&
        mkdir "${GTest_INSTALL_DIR}/lib" && cd lib &&
        cp "libgtest.a" "${GTest_INSTALL_DIR}/lib/" &&
        cp "libgtest_main.a" "${GTest_INSTALL_DIR}/lib/"
        INSTALL_DIR "${GTest_INSTALL_DIR}"
)
add_dependencies(external-downloads GTest-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTest DEFAULT_MSG GTest_INCLUDE_DIRS)