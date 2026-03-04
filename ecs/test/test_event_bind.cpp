
#include <thread>
#include <chrono>
#include <print>

import nx.platform.win32.event_looop.event;
import nx.platform.win32.event_loop;


using namespace std::chrono_literals;

int main()
{
    nx::Win32EventLoop eventLoop{};



    std::jthread thread{ [&]()
    {
        eventLoop.Run();
    } };


    eventLoop.On<nx::CloseEvent>([](const nx::CloseEvent& e)
    {
        std::println("close {}", reinterpret_cast<intptr_t>(e.window));

    });

    eventLoop.On<nx::ResizeEvent>([](const nx::ResizeEvent& e)
    {
        std::println("Resize {},{},{}", reinterpret_cast<intptr_t>(e.window), e.width, e.height);

    });

    std::this_thread::sleep_for(500ms);


    nx::CloseEvent closeEvent{};
    closeEvent.window = reinterpret_cast<nx::IWindow*>(123);


    nx::ResizeEvent resizeEvent{};
    resizeEvent.window = reinterpret_cast<nx::IWindow*>(456);
    resizeEvent.width = 100;
    resizeEvent.height = 1002;

    eventLoop.Post(closeEvent);
    eventLoop.Post(resizeEvent);


}
