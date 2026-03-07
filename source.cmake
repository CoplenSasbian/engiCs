

macro(target_sources_by_extension target_name )
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

    if (source_files OR header_files)
        target_sources(${target_name}
                PRIVATE ${source_files}
                PUBLIC
                FILE_SET cxx_headers TYPE HEADERS
                BASE_DIRS .
                FILES ${header_files}
        )
    endif ()

    if (module_files)
        target_sources(${target_name} PUBLIC
                FILE_SET cxx_modules TYPE CXX_MODULES
                BASE_DIRS .
                FILES ${module_files}
        )
    endif ()


endmacro()