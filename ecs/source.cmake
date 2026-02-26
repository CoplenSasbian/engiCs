
set(ECS_MODULE

        "concurrency/threadpool/threadpool.ixx"
        "concurrency/utils/pause.ixx"
        "concurrency/utils/processor_info.ixx"
        "concurrency/event/event.ixx"
        "concurrency/future/function_proxy.ixx"


        "core/config/console_config.ixx"
        "core/container/hive.ixx"
        "core/exception/exception.ixx"
        "core/log/log.ixx"
        "core/log/sqlite_sink.ixx"
        "core/memory/memory.ixx"
        "core/types/class_def.ixx"
        "core/types/t_string.ixx"


        "platform/window/window.ixx"
        "platform/window/eventloop.ixx"
        "platform/window/win32/win32_window.ixx"
        "platform/window/win32/win32_event_loop.ixx"

        "core/utils/single.ixx"
        "core/utils/emitter.ixx"
        "core/utils/function_traits.ixx"


        "resource/actor.ixx"

        "rhi/device.ixx"
        "rhi/queue.ixx"
        "rhi/surface.ixx"
        "rhi/shader.ixx"

        "rhi/vulkan/config.ixx"
        "rhi/vulkan/vulkan_allocator.ixx"
        "rhi/vulkan/vulkan_device.ixx"
        "rhi/vulkan/vulkan_queue.ixx"
        "rhi/vulkan/vulkan_shader.ixx"

        "game/app/app.ixx"
        "game/context/context.ixx"
        "game/render_window/render_window.ixx"


)

set(ECS_SOURCE

        "core/container/concurrent_queue.h"
        "concurrency/threadpool/threadpool.cpp"
        "concurrency/utils/pause.cpp"
        "concurrency/utils/processor_info.cpp"
        "concurrency/event/event.cpp"

        "core/config/console_config.cpp"
        "core/exception/exception.cpp"
        "core/log/log.h"
        "core/log/log.cpp"
        "core/log/sqlite_sink.cpp"
        "core/memory/memory.cpp"

        "platform/window/win32/win32_window.cpp"
        "platform/window/window.cpp"
        "platform/window/eventloop.cpp"
        "platform/window/win32/win32_event_loop.cpp"

        "rhi/vulkan/config.cpp"
        "rhi/vulkan/vulkan_allocator.cpp"
        "rhi/vulkan/vulkan_device.cpp"
        "rhi/vulkan/vulkan_queue.cpp"
        "rhi/vulkan/vulkan_shader.cpp"


        "game/app/app.cpp"
        "game/context/context.cpp"
        "game/render_window/render_window.cpp"

        "ecs.cpp"

)
