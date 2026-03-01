set(esc_lib_resource_sources
        "actor.ixx"
)

add_libary_with_deps(esc_lib_resource ${esc_lib_resource_sources})

target_link_libraries(esc_lib_resource
        PRIVATE
        esc_lib_core
)