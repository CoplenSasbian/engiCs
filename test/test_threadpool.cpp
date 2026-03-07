//
// Created by futurvo on 2026/3/1.
//

#include <chrono>
#include <print>
#include <future>
#include <memory_resource>
#include "concurrency/threadpool/threadpool.h"

int main()
{
    using namespace std::chrono_literals;
    std::pmr::set_default_resource(std::pmr::new_delete_resource());
    nx::Threadpool pool{4};


    auto make_timer = pool.MakeTimer();


    std::promise<void> p;

    make_timer.SetCallback([&]
    {
        printf("cb\n");
        static  int i = 0;
        if (++i > 10)
            p.set_value();
    });

    make_timer.Start(1s,1s);

    p.get_future().wait();
}