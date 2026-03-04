set(esc_lib_core_sources

        "error_code/error_code.ixx"
        "config/console_config.ixx"
        "container/hive.ixx"
        "exception/exception.ixx"
        "log/log.ixx"
        "log/sqlite_sink.ixx"
        "memory/memory.ixx"
        "types/class_def.ixx"
        "types/t_string.ixx"
        "utils/single.ixx"
        "utils/emitter.ixx"
        "utils/function_traits.ixx"
        "utils/sso_any.ixx"


        "container/concurrent_queue.h"
        "config/console_config.cpp"
        "exception/exception.cpp"
        "log/log.h"
        "log/log.cpp"
        "log/sqlite_sink.cpp"
        "memory/memory.cpp"

)


add_libary_with_deps(esc_lib_core ${esc_lib_core_sources})

target_link_libraries(esc_lib_core
        PRIVATE
        mimalloc
        unofficial::sqlite3::sqlite3
        spdlog::spdlog
)
target_include_directories(
        esc_lib_core
        PUBLIC
        ${SOL2_INCLUDE_DIRS}
        ${SQLITE_MODERN_CPP_INCLUDE_DIRS}
)

