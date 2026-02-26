#include <print>
#include <thread>
#include <chrono>
#include <future>
#include <ostream>
#include <exec/task.hpp>
#include <exec/static_thread_pool.hpp>
#include <stdexec/execution.hpp>
#include <exec/async_scope.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


import nx.concurrency.threadpool;
import nx.concurrency.utils.processor_info;
import nx.concurrency.utils.event;
import nx.core.config.console;
import nx.concurrency.utils.pause;
import nx.concurrency.promise;
import nx.core.memory;

namespace std::execution {
    using namespace stdexec;
}

exec::task<void> pF(nx::EventSource& es) {

    int ms = std::rand()%1000;



    std::println("this thread {}",std::this_thread::get_id());
    co_return;
}

int main(int argc, char **argv) {
    using namespace std::chrono_literals;
    nx::SetupConsole();
    nx::InitResource();
    // game::App app{argc,argv};
    // app.Run();
    try {
        auto masks = nx::GetCoreMask(nx::High);
        nx::Threadpool pool{
            masks.size(),
            [&](auto index) {
                ::SetThreadAffinityMask(GetCurrentThread(), masks[index]);
            },
            nx::GetCommandResource()
        };

        nx::EventSource es;


        auto entry = [&]()->exec::task<void> {
                exec::async_scope scope{};
            std::execution::scheduler  auto scheduler = pool.get_scheduler();
            for (int i = 0; i < 100; ++i) {
                try {
                    auto task = std::execution::schedule(scheduler)
                    |std::execution::let_value([&]{return pF(es);})
                    |std::execution::upon_stopped([] {
                        std::println("task stop!");
                        return std::execution::just();
                    })
                    |std::execution::upon_error([](std::exception_ptr ptr) {
                        std::println("task error!");
                        return std::execution::just();
                    });
                    // std::execution::sync_wait(std::move(task));
                    scope.spawn(std::move(task));
                }catch(std::exception &e) {
                    std::println("Exception: {}", e.what());
                }

            }
            scope.request_stop();
            co_await scope.on_empty();

        };

        std::execution::sync_wait(
          entry()
        );


    } catch (std::exception &e) {
        std::println("Exception: {}\n", e.what());
    }


    return 0;
}
