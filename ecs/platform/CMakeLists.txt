set(esc_lib_platform_sources
        "window/window.ixx"
        "window/eventloop.ixx"
        "window/win32/win32_window.ixx"
        "window/win32/win32_event_loop.ixx"

        "window/win32/win32_window.cpp"
        "window/window.cpp"
        "window/eventloop.cpp"
        "window/win32/win32_event_loop.cpp"
)

add_libary_with_deps(esc_lib_platform ${esc_lib_platform_sources})
target_link_libraries(esc_lib_platform
        PRIVATE
        esc_lib_core
        Boost::container
)
