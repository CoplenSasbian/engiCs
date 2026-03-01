
macro(add_libary_with_deps target_name)
    add_library(${target_name} STATIC)

    target_compile_definitions(${target_name} PRIVATE
            NOMINMAX
            UNICODE
            _UNICODE
    )

    target_include_directories(
            ${target_name}
            PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
    )

    set(all_extra_sources ${ARGN})

    set(module_files "")
    set(header_files "")
    set(source_files "")

    foreach (f ${all_extra_sources})
        if (f MATCHES "\\.(ixx|cppm|mpp)$")
            list(APPEND module_files ${f})
        elseif (f MATCHES "\\.(h|hpp|hxx)$")
            list(APPEND header_files ${f})
        else ()
            list(APPEND source_files ${f})
        endif ()
    endforeach ()

    if (source_files)
        target_sources(${target_name}
                PRIVATE ${source_files}
                PUBLIC ${header_files}
        )
    endif ()

    if (module_files)
        target_sources(${target_name} PUBLIC
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


add_subdirectory(core)
add_subdirectory(concurrency)
add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(resource)
add_subdirectory(game)
