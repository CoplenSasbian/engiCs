set(esc_lib_concurrency_sources
        "erroe_code/error_code.ixx"
        "threadpool/threadpool.ixx"
        "utils/pause.ixx"
        "utils/processor_info.ixx"
        "run_loop/event_loop.ixx"
        "run_loop/worker_loop.ixx"
        "future/function_proxy.ixx"

        "threadpool/Threadpool.cpp"
        "utils/pause.cpp"
        "utils/processor_info.cpp"
        "run_loop/event_loop.cpp"
        "run_loop/worker_loop.cpp"
)

add_libary_with_deps(esc_concurrency ${esc_lib_concurrency_sources})
target_link_libraries(esc_concurrency
        PRIVATE
        Boost::asio
        esc_core
        PUBLIC
        STDEXEC::stdexec
)
target_compile_definitions(esc_concurrency
        PUBLIC
        ASIO_IMPL
)

