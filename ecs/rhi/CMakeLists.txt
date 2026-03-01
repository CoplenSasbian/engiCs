set(esc_lib_rhi_sources

        "device.ixx"
        "queue.ixx"
        "surface.ixx"
        "shader.ixx"

        "vulkan/config.ixx"
        "vulkan/vulkan_allocator.ixx"
        "vulkan/vulkan_device.ixx"
        "vulkan/vulkan_queue.ixx"
        "vulkan/vulkan_shader.ixx"

        "vulkan/config.cpp"
        "vulkan/vulkan_allocator.cpp"
        "vulkan/vulkan_device.cpp"
        "vulkan/vulkan_queue.cpp"
        "vulkan/vulkan_shader.cpp"
)

add_libary_with_deps(esc_lib_rhi ${esc_lib_rhi_sources})
target_link_libraries(esc_lib_rhi
        PRIVATE
        esc_lib_core
        Vulkan::Vulkan

)
