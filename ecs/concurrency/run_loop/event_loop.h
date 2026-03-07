#pragma once
#include <chrono>
#include <functional>

#include "core/error_code.h"

namespace nx
{
    using Task = std::function<void()>;

  class Timer ;

    class EventLoop
    {
    public:
        EventLoop(std::pmr::memory_resource* = std::pmr::get_default_resource() ) noexcept;
        void Shutdown() noexcept ;
        void Run() noexcept;
        void RunOnce() noexcept;
        Error PostTask(Task&&) noexcept;
        ~EventLoop() noexcept ;
    private:
        struct IMPL;
        IMPL* m_impl;

        std::pmr::memory_resource* m_memResource;
        friend class Timer;
    };


    class Timer
    {
    public:
        using Duration = std::chrono::milliseconds;
        Timer(EventLoop* es) noexcept;
        Timer(Timer&&)noexcept =default;
        Timer& operator=(Timer&&)noexcept =default;
        void SetCallback(Task&&) noexcept;
        Error Start(Duration delay, Duration repeat = Duration(0)) noexcept;
        void Run() const noexcept;
        void Stop() noexcept;
        void Shutdown() noexcept ;
        ~Timer() noexcept ;

    private:
        struct IMPL;
        IMPL* m_impl;
        EventLoop* m_es;
        Task m_cb;
    };
}
