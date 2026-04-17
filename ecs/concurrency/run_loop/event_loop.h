#pragma once
#include <chrono>
#include <functional>

#include "core/error_code.h"
#include "concurrency/run_loop/task.h"
namespace nx
{

  class Timer ;

    class EventLoop
    {
    public:
        EventLoop(std::pmr::memory_resource* = std::pmr::get_default_resource() ) noexcept;
        void Shutdown() noexcept ;
        void Run() noexcept;
        void RunOnce() noexcept;
		template<TaskCallable Callable>
        Error PostTask(Callable&& c) noexcept {
			auto taskWrapper = MakeTask(std::forward<Callable>(c), m_memResource);
			auto errc = PostTask(taskWrapper);
            if (!errc) {
				taskWrapper->Destroy();
            }
            return errc;
        }
        Error PostTask(Task* task) noexcept;
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

		template<TaskCallable Callable>
        void SetCallback(Callable&& c) noexcept {
			auto task = nx::MakeTask(std::forward<Callable>(c), m_es->m_memResource);
			ResetCallback(task);
        }


        Error Start(Duration delay, Duration repeat = Duration(0)) noexcept;
        void Run() const noexcept;
        void Stop() noexcept;
        void Shutdown() noexcept ;
        ~Timer() noexcept ;

    private:
		void ResetCallback(Task* task = nullptr) noexcept;
        struct IMPL;
        void ResetImpl(IMPL* newImpl = nullptr) noexcept;
        IMPL* m_impl;
        EventLoop* m_es;
        Task* m_cb;
    };


}
