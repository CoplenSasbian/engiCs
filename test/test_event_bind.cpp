
#include <thread>
#include <chrono>
#include <print>

#include "platform/win32/event_loop/message_loop.h"
#include "platform/win32/window/window.h"
#include "platform/win32/event_loop/event_converter.h"

using namespace std::chrono_literals;

int main()
{
    constexpr  static auto pl = nx::PlatformWin32;
    nx::MessageLoop<pl> eventLoop{};

    nx::Window<pl> window{};
    if (auto err= window.Create(nx::String::from("app"), { 100,100,800,600 }))
    {
        std::println(stderr,"Create window error： {}",err.value().message());
        abort();
    }
    window.SetVisible(true);
    eventLoop.On<nx::CloseEvent>([&](const nx::CloseEvent& e)
    {
        window.Destroy();
        std::println("close {}", reinterpret_cast<intptr_t>(e.window));
        eventLoop.PostQuit();
    });

    eventLoop.On<nx::ResizeEvent>([](const nx::ResizeEvent& e)
    {
        std::println("Resize {},{},{}", reinterpret_cast<intptr_t>(e.window), e.width, e.height);
    });


    eventLoop.Run();











}
