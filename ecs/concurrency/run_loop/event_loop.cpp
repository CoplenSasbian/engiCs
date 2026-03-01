module;
#include <boost/asio.hpp>
#include "log/log.h"
module nx.concurrency.run_loop.event_source;

import nx.core.log;
LOGGER(event_loop);


struct nx::EventLoop::IMPL
{
    IMPL()
    :m_ctx{},m_work(boost::asio::make_work_guard(m_ctx)){}

    boost::asio::io_context m_ctx;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work;
};

nx::EventLoop::EventLoop(std::pmr::memory_resource* res) noexcept
:m_memResource(res)
{
    std::pmr::polymorphic_allocator<IMPL> alloc{res};
    m_impl = alloc.new_object<IMPL>();
}


void nx::EventLoop::Shutdown() noexcept
{
    if (m_impl->m_ctx.stopped()) return;
    m_impl->m_work.reset();
    m_impl->m_ctx.stop();
}

void nx::EventLoop::Run() noexcept
{
    m_impl->m_ctx.run();
}

void nx::EventLoop::RunOnce() noexcept
{
    m_impl->m_ctx.run_one();
}

std::optional<nx::NxError> nx::EventLoop::PostTask(Task&& task) noexcept
{
   if (m_impl->m_ctx.stopped())
   {
       return NxError{"event loop is stopped"};
   }

    boost::asio::post(m_impl->m_ctx, std::move(task));
    return std::nullopt;
}

nx::EventLoop::~EventLoop() noexcept
{
    EventLoop::Shutdown();
    std::pmr::polymorphic_allocator<IMPL> alloc{m_memResource};
    alloc.delete_object(m_impl);
}




struct nx::Timer::IMPL
{
    IMPL(boost::asio::io_context& ctx)
    :m_timer(ctx)
    {}
    boost::asio::steady_timer m_timer;
};

nx::Timer::Timer(EventLoop* es) noexcept
:m_es(es)
{
    std::pmr::polymorphic_allocator<> alloc;
    m_impl = alloc.new_object<IMPL>(m_es->m_impl->m_ctx);
}



void nx::Timer::SetCallback(Task&& cb) noexcept
{
    m_cb = std::move(cb);
}

std::optional<nx::NxError> nx::Timer::Start(Duration delay, Duration repeat) noexcept
{
    m_impl->m_timer.expires_after(delay);
    struct TimerCallback
    {
        Timer * m_timer;
        Duration m_repeat;
        void operator()(const boost::system::error_code& ec) noexcept
        {
            if (ec) [[unlikely]]
            {
                _logger.Error(std::format("timer error: {}", ec.message()));
                return;
            }
            m_timer->Run();
            if (m_repeat != Duration::zero())
            {
                m_timer->m_impl->m_timer.expires_after(m_repeat);
                m_timer->m_impl->m_timer.async_wait(*this);
            }
        }
    };

    m_impl->m_timer.async_wait(TimerCallback{this,repeat});

    return std::nullopt;
}

void nx::Timer::Run() const noexcept
{
    assert(m_cb);
    m_cb();
}

void nx::Timer::Stop() noexcept
{
    m_impl->m_timer.cancel();
}

void nx::Timer::Shutdown() noexcept
{
    Stop();
}

nx::Timer::~Timer() noexcept
{
    Shutdown();
    std::pmr::polymorphic_allocator<IMPL> alloc{m_es->m_memResource};
    alloc.delete_object(m_impl);
    m_impl = nullptr;
}



