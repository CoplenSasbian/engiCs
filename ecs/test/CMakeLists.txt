
macro(add_executable_with_deps target_name)
    set(main_source "${target_name}.cpp")
    add_executable(${target_name} ${main_source})

    set(all_extra_sources ${ARGN})

    set(module_files "")
    set(source_files "")

    foreach (f ${all_extra_sources})
        if (f MATCHES "\\.(ixx|cppm|mpp)$")
            list(APPEND module_files ${f})
        else ()
            list(APPEND source_files ${f})
        endif ()
    endforeach ()

    if (source_files)
        target_sources(${target_name} PRIVATE ${source_files})
    endif ()

    if (module_files)
        target_sources(${target_name} PRIVATE
                FILE_SET cxx_modules TYPE CXX_MODULES
                BASE_DIRS ..
                FILES ${module_files}
        )

    endif ()

    set_target_properties(${target_name} PROPERTIES
            CXX_STANDARD 23
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            CXX_SCAN_FOR_MODULES ON
    )
endmacro()


add_executable_with_deps(
        test_event_source
)

target_link_libraries(test_event_source
        esc_lib_concurrency
)



add_executable_with_deps(
        test_threadpool
)

target_link_libraries(test_threadpool
        esc_lib_concurrency
)

