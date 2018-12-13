set(Glog_INCLUDE_DIRS ${Glog_INSTALL_DIR}/include)
set(Glog_LIBRARIES ${Glog_INSTALL_DIR}/lib${LIBSUFFIX}/libglog.a)
set(Glog_EXTERNAL TRUE)

message(STATUS "Removing remains of previously downloaded Glog versions")
file(REMOVE_RECURSE ${Glog_DOWNLOAD_DIR})
file(REMOVE_RECURSE ${Glog_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
        Glog-download
        PREFIX ${Glog_DOWNLOAD_DIR}
        GIT_REPOSITORY https://github.com/google/glog.git
        UPDATE_COMMAND ""
        BUILD_COMMAND cmake -H. -Bbuild -G "Unix Makefiles" && cmake --build build
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND
        cd build &&
        #mv ../include "${Glog_INSTALL_DIR}/include" &&
        mkdir "${Glog_INSTALL_DIR}/lib" && #cd lib &&
        mv "libglog.a" "${Glog_INSTALL_DIR}/lib${LIBSUFFIX}/"
        INSTALL_DIR "${Glog_INSTALL_DIR}"
)

#CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
#GIT_TAG "" #d0531421fd5437ae3e5249106c6fc4247996e526 # Working master at this point in time

add_dependencies(external-downloads Glog-download)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Glog DEFAULT_MSG Glog_INCLUDE_DIRS Glog_LIBRARIES)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTest DEFAULT_MSG GTest_INCLUDE_DIRS)
#
#ExternalProject_Get_Property(Glog-download source_dir install_dir)
#
## Only build on demand
#set_target_properties(Glog-download PROPERTIES EXCLUDE_FROM_ALL TRUE)
#
#file(MAKE_DIRECTORY "${install_dir}/include")
#
##set(${package_found_prefix}_CMAKE_DEP glog_external)
#if(APPLE)
#    set(${external_dependency}_LIBRARIES "${install_dir}/lib/libglog.a")
#elseif(UNIX)
#    set(${external_dependency}_LIBRARIES "${install_dir}/lib${LIBSUFFIX}/libglog.a")
#endif()
#set(${external_dependency}_INCLUDE_DIRS "${install_dir}/include")
