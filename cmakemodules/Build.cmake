include(Dependency)

macro(build_library)
    message(STATUS "Adding library '${PROJECT_NAME}'")
    #
    #    set(options)
    #    set(oneValueArgs)
    #    set(multiValueArgs DEPS)
    #    cmake_parse_arguments(SOURCE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
    target_link_libraries(${PROJECT_NAME} "-lm")
    #add_dependency(${PROJECT_NAME} "SDSL" REQUIRED)
    add_dependency(${PROJECT_NAME} "OpenMP" REQUIRED)
    add_dependency(${PROJECT_NAME} "Glog" REQUIRED)
    add_definitions(-D_GLIBCXX_PARALLEL)
    message(STATUS "Library '${PROJECT_NAME}' successfully added")
endmacro(build_library)

macro(build_executable name)
    message(STATUS "Adding executable '${name}'")

    add_executable(${name} src/tir_driver.cpp src/main_${name}.cpp)
    target_link_libraries(${name} ${PROJECT_NAME})

    install(TARGETS ${name} RUNTIME DESTINATION build)
    message(STATUS "Executable '${name}' successfully added")
endmacro(build_executable)

macro(build_executable_mem name)
    message(STATUS "Adding executable '${name}_mem'")

    file(GLOB_RECURSE SOURCES src/main_${name}.cpp)
    add_executable(${name}_mem ${SOURCES} include/malloc_count.h src/malloc_count.c)
    target_link_libraries(${name}_mem ${PROJECT_NAME})
    target_link_libraries(${name}_mem -ldl)

    install(TARGETS ${name} RUNTIME DESTINATION build)
    message(STATUS "Executable '${name}' successfully added")
endmacro(build_executable_mem)

macro(build_tests name)
    if (${BUILD_TESTS})
        message(STATUS "Adding tests for '${name}'")

        file(GLOB_RECURSE TESTS test/test_driver.cpp test/${name}_test.cpp)
        add_executable(${name}_test ${TESTS})
        target_link_libraries(${name}_test ${PROJECT_NAME})
        add_dependency(${name}_test GTest)
        install(TARGETS ${name}_test RUNTIME DESTINATION build)

        message(STATUS "Tests for '${name}' successfully added")
    endif ()
endmacro(build_tests)

macro(build_target_with_test name)
    build_executable(${name})
    build_tests(${name})
endmacro(build_target_with_test)
