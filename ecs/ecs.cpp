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

std::atomic_int pfC = 0;
std::atomic_int pfC2 = 0;

exec::task<void> pF2(nx::Threadpool& pool) noexcept
{
    auto id = std::this_thread::get_id();
    auto ms = std::chrono::milliseconds(20 + std::rand() % 3000i64);
    auto now = std::chrono::high_resolution_clock::now();
    co_await nx::timeout(pool, ms);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now);
    std::println("2 from {} to {}  expect {} in fact {}", id, std::this_thread::get_id(), ms, dur);
    ++pfC2;
};


exec::task<void> pF(nx::Threadpool& pool) noexcept
{
    auto id = std::this_thread::get_id();
    auto ms = std::chrono::milliseconds(20 + std::rand() % 3000i64);
    auto now = std::chrono::high_resolution_clock::now();
    co_await nx::timeout(pool, ms);
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now);
    std::println("from {} to {}  expect {} in fact {}", id, std::this_thread::get_id(), ms, dur);

    exec::async_scope scope;
    for (int i = 0; i < 200; ++i)
    {
        scope.spawn(nx::StartsOnThreadPool(pool,pF2(pool)));
    }
    co_await scope.on_empty();
    ++pfC;
    co_return;
}

int main(int argc, char** argv)
{
    mi_version();
    nx::InitResource();
    nx::SetupConsole();
    // game::App app{argc,argv};
    // app.Run();

    auto res = nx::GetCoreMask(nx::High);

    if (res.has_value())
    {
        auto masks = res.value();
        nx::Threadpool pool{
            static_cast<int>(masks.size()),
            [&](auto index)
            {
                nx::SetCurrentThreadAffinity(masks[index]);
            },
            nx::GetCommandResource()
        };

        auto entry = [&]()-> exec::task<void>
        {
            exec::async_scope scope{};
            for (int i = 0; i < 100; ++i)
            {
                auto task =
                    nx::BindThreadStartsOnThreadPool(pool, pF(pool)) |
                    std::execution::upon_error([](auto&& e)
                    {
                        try { std::rethrow_exception(e) ;}
                        catch (std::exception& e)
                        {
                            std::println("Exception: {}", e.what());
                        }
                    });
                scope.spawn(std::move(task));
            }
            co_await scope.on_empty();
        };

        std::execution::sync_wait(
            entry()
        );

        std::println("{} {}",pfC.load(),pfC2.load());
    }
    else
    {
        return -1;
    }




    return 0;
}
