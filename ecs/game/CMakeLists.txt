
set(esc_lib_game_sources
        "app/app.ixx"
        "context/context.ixx"
        "render_window/render_window.ixx"

        "app/app.cpp"
        "context/context.cpp"
        "render_window/render_window.cpp"
)
add_libary_with_deps(esc_lib_game ${esc_lib_game_sources})
target_link_libraries(esc_lib_game
        PRIVATE
        esc_lib_core
        esc_lib_concurrency
        esc_lib_platform
)
