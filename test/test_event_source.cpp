//
// Created by futurvo on 2026/2/27.
//

#include <chrono>
#include <print>
#include <thread>
#include <memory_resource>
#include "concurrency/run_loop/event_loop.h"

int main()
{
    using namespace std::chrono_literals;
    std::pmr::set_default_resource(std::pmr::new_delete_resource());
    nx::EventLoop es;


    nx::Timer ts{
        &es
    };
    ts.SetCallback([&]
    {
        printf("cb\n");
        es.Shutdown();
    });

    std::jthread t{
        [&]()
        {
            std::this_thread::sleep_for(1s);
            auto res = ts.Start(1s, 1s);
        }
    };
    t.detach();

    es.Run();

    printf("exit\n");
}
