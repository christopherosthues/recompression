set(sparsepp_INCLUDE_DIRS ${sparsepp_INSTALL_DIR}/include)
set(sparsepp_INCLUDE_DIRS ${sparsepp_INCLUDE_DIRS} ${sparsepp_INSTALL_DIR}/include/internal)
set(sparsepp_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded sparsepp versions")
file(REMOVE_RECURSE ${sparsepp_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${sparsepp_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        sparsepp-download
        PREFIX ${sparsepp_DOWNLOAD_DIR}
        GIT_REPOSITORY "https://github.com/edawson/sparsepp.git"
        GIT_TAG "1c5a285e87b2244383efe0398f9992acd61234e9"
        BUILD_IN_SOURCE TRUE
#        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} # TODO ADD static build flag
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_COMMAND ""
        CONFIGURE_COMMAND ""
        INSTALL_COMMAND cp -r "sparsepp/." "${sparsepp_INSTALL_DIR}/include/internal"
        INSTALL_DIR "${sparsepp_INSTALL_DIR}"
)

add_dependencies(external-downloads sparsepp-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(sparsepp DEFAULT_MSG sparsepp_INCLUDE_DIRS)
