set(esc_lib_platform_sources
        "error_code/error_code.ixx"
        "platform.ixx"

        "event_loop/event_loop.ixx"
        "event_loop/event_loop.cpp"
        "event_loop/event.ixx"
        "window/window.ixx"




        "win32/win32_platform.ixx"

        "win32/event_loop/event_loop.ixx"
        "win32/event_loop/event_loop.cpp"
        "win32/event_loop/event_converter.ixx"
        "win32/event_loop/event_converter.cpp"

        "win32/window/window.ixx"
        "win32/window/window.cpp"

)
add_libary_with_deps(esc_lib_platform ${esc_lib_platform_sources})
target_link_libraries(esc_lib_platform
        PUBLIC
        esc_lib_core
        Boost::container
)
