get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

if ("${LIB64}" STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else ()
    set(LIBSUFFIX "")
endif ()

macro(add_dependency target external_dependency)
    if (NOT ${target}_DEPENDS_ON_${external_dependency})

        message(STATUS)
        message(STATUS "Adding dependency from '" ${external_dependency} "' for target '" ${target} "'")

        set(additional_args "${ARGN}")

        if (${external_dependency} STREQUAL "GTest")
            list(FIND additional_args "Main" indexOfMain)
            if (${indexOfMain} GREATER -1)
                set(GTest_IsMain true)
                list(REMOVE_AT additional_args ${indexOfMain})
                message(STATUS "The default GTest main-method will be used.")
            else ()
                set(GTest_IsMain false)
            endif ()
        endif ()

        if (NOT ${external_dependency}_FOUND)
            find_package(${external_dependency} ${additional_args})
        endif ()
        
        include_directories(${${external_dependency}_INCLUDE_DIRS})
        target_link_libraries(${target} ${${external_dependency}_LIBRARIES})
        
        if (${external_dependency}_EXTERNAL)
            message(STATUS "Adding dependencies of ${external_dependency}")
            add_dependencies(${target} ${external_dependency}-download)
        endif ()

        if (${external_dependency} STREQUAL "GTest")
            find_package(Threads REQUIRED)
            target_link_libraries(${target} Threads::Threads)
            if (GTest_IsMain)
                target_link_libraries(${target} ${GTest_Main_LIBRARIES})
            endif ()
            append_compile_flags(${target} "-pthread")
        endif ()

        append_compile_flags(${target} "-fopenmp")
        append_link_flags(${target} "-fopenmp")

        set(${target}_DEPENDS_ON_${external_dependency} true)
    endif ()
endmacro()

macro(append_compile_flags target compile_flags)
    get_property(current_property_flags TARGET ${target} PROPERTY COMPILE_FLAGS)
    set(current_property_flags "${current_property_flags} ${compile_flags}")
    set_target_properties(${target} PROPERTIES COMPILE_FLAGS ${current_property_flags})
endmacro()

macro(append_link_flags target compile_flags)
    get_property(current_property_flags TARGET ${target} PROPERTY LINK_FLAGS)
    set(current_property_flags "${current_property_flags} ${compile_flags}")
    set_target_properties(${target} PROPERTIES LINK_FLAGS ${current_property_flags})
endmacro()
