#include <print>
#include <thread>
#include <chrono>
#include <future>
#include <ostream>
#include <exec/task.hpp>
#include <exec/static_thread_pool.hpp>
#include <stdexec/execution.hpp>
#include <exec/async_scope.hpp>

#include <mimalloc.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


import nx.concurrency.threadpool;
import nx.concurrency.utils.processor_info;
import nx.core.config.console;
import nx.concurrency.utils.pause;
import nx.concurrency.promise;
import nx.core.memory;

namespace std::execution
{
    using namespace stdexec;
}

    using namespace std::chrono_literals;

exec::task<void> pF(nx::Threadpool& pool)
{
    auto id = std::this_thread::get_id();
    auto ms = 500 + std::rand() % 1000i64;
    auto now = std::chrono::high_resolution_clock::now();
    co_await nx::timeout(pool,std::chrono::milliseconds(ms));

    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now);

    std::println("from {} to {}  expect {}ms act {}ms",id, std::this_thread::get_id(),ms,dur);
    co_return;
}

int main(int argc, char** argv)
{
    mi_version();
    nx::InitResource();
    nx::SetupConsole();
    // game::App app{argc,argv};
    // app.Run();

    auto masks = nx::GetCoreMask(nx::High);
    nx::Threadpool pool{
        static_cast<int>(masks.size()),
        [&](auto index)
        {
            ::SetThreadAffinityMask(GetCurrentThread(), masks[index]);
        },
        nx::GetCommandResource()
    };

    auto entry = [&]()-> exec::task<void>
    {
        exec::async_scope scope{};
        std::execution::scheduler auto scheduler = pool.get_scheduler();
        for (int i = 0; i < 100; ++i)
        {
            try
            {
                auto task =
                   stdexec::on(scheduler, pF(pool));
                scope.spawn(std::move(task));
            }
            catch (std::exception& e)
            {
                std::println("Exception: {}", e.what());
            }
        }
        co_await scope.on_empty();
    };

    std::execution::sync_wait(
        entry()
    );


    return 0;
}
