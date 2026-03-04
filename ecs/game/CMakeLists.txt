
set(esc_lib_game_sources
        "error_code/error_code.ixx"
        "app/app.ixx"
        "render_window/render_window.ixx"

        "app/app.cpp"
        "render_window/render_window.cpp"
)
add_libary_with_deps(esc_lib_game ${esc_lib_game_sources})
target_link_libraries(esc_lib_game
        esc_lib_concurrency
        esc_lib_platform
)
