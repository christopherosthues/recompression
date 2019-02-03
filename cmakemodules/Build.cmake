include(Dependency)

macro(build_library)
    message(STATUS "Adding library '${PROJECT_NAME}'")

    add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
    target_link_libraries(${PROJECT_NAME} "-lm")
    add_dependency(${PROJECT_NAME} "DYNAMIC" REQUIRED)
    add_dependency(${PROJECT_NAME} "IPS4o" REQUIRED)
    add_dependency(${PROJECT_NAME} "OpenMP" REQUIRED)
    add_definitions(-D_GLIBCXX_PARALLEL)

    target_link_libraries(${PROJECT_NAME} -latomic)

    message(STATUS "Library '${PROJECT_NAME}' successfully added")
endmacro(build_library)

macro(build_bench name)
    message(STATUS "Adding executable 'bench_${name}'")

    add_executable(bench_${name} ${name}_bench.cpp)
    target_link_libraries(bench_${name} ${PROJECT_NAME})
    add_dependency(bench_${name} "OpenMP" REQUIRED)

    if(ENABLE_MALLOC_COUNT)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
        target_sources(bench_${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/malloc_count)
        target_compile_definitions(bench_${name} PUBLIC MALLOC_COUNT)
    endif(ENABLE_MALLOC_COUNT)

    install(TARGETS bench_${name} RUNTIME DESTINATION build)
    message(STATUS "Executable 'bench_${name}' successfully added")
endmacro(build_bench)

#macro(build_executable_mem name)
#    message(STATUS "Adding executable '${name}_mem'")
#
#    file(GLOB_RECURSE SOURCES src/main_${name}.cpp)
#    add_executable(${name}_mem ${SOURCES} include/malloc_count.h src/malloc_count.c)
#    target_link_libraries(${name}_mem ${PROJECT_NAME})
#    target_link_libraries(${name}_mem -ldl)
#
#    install(TARGETS ${name} RUNTIME DESTINATION build)
#    message(STATUS "Executable '${name}' successfully added")
#endmacro(build_executable_mem)

macro(build_test name)
    message(STATUS "Adding tests for '${name}'")

    file(GLOB_RECURSE TESTS test_driver.cpp ${name}_test.cpp)
    add_executable(${name}_test ${TESTS})
    target_link_libraries(${name}_test ${PROJECT_NAME})
    add_dependency(${name}_test "GTest" REQUIRED)
    install(TARGETS ${name}_test RUNTIME DESTINATION build)

    if (RECOMPRESSION_ENABLE_AUTO_RUN_TESTS)
#        add_test(${name}_test ${name}_test)
        add_custom_command(TARGET ${name}_test POST_BUILD COMMAND $<TARGET_FILE:${name}_test>)
    endif (RECOMPRESSION_ENABLE_AUTO_RUN_TESTS)

    message(STATUS "Tests for '${name}' successfully added")
endmacro(build_test)

