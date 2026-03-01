module;
#include <atomic>
#include <chrono>
#include <thread>
#include <latch>
#include "log/log.h"
#include "container/concurrent_queue.h"
#include "exec/env.hpp"

module nx.concurrency.threadpool;
import nx.core.exception;
import nx.concurrency.utils.pause;
import nx.core.log;
import nx.core.memory;
LOGGER(thread_pool);


thread_local int tCurrentThreadToken = -1;


nx::Threadpool::Threadpool(int threadNums, std::optional<std::function<void(size_t index)>> cb,
                           std::pmr::memory_resource* resource)
    : m_resource(resource), m_workerLoop(threadNums, resource),
      m_workerThreads(resource),
      m_threadNums(threadNums),
      m_threadInit(std::move(cb))
{
    m_workerThreads.reserve(m_threadNums);
    std::latch latch(static_cast<ptrdiff_t>(m_threadNums) + 1);

    for (int i = 0; i < m_threadNums; i++)
    {
        m_workerThreads.emplace_back(
            [this,i,&latch]()
            {
                if (m_threadInit) m_threadInit->operator()(i);
                tCurrentThreadToken = i;
                latch.count_down();
                m_workerLoop.Run(i);
            }
        );
    }

    m_esThread = std::jthread{
        [this,&latch]()
        {
            tCurrentThreadToken = static_cast<int>(m_threadNums);
            latch.count_down();
            m_es.Run();
        }
    };
    latch.wait();
}


nx::Threadpool::~Threadpool()
{
    m_workerLoop.Shutdown();
    m_es.Shutdown();
}


void nx::Threadpool::Shutdown()
{
}

std::optional<nx::NxError> nx::Threadpool::PostTask(Task&& task, PostConfig config) noexcept
{
    std::optional<NxError> ret{};
    std::visit([&]<typename T0>(T0& c)
    {
        if constexpr (std::is_same_v<T0, TheadToken>)
        {
            if (c == m_threadNums)
            {
                ret = m_es.PostTask(std::move(task));
            }
            else [[likely]]
            {
                ret = m_workerLoop.PostTask(std::move(task), c);
            }
        }
        else
        {
            if (c == ThreadType::Any)
            {
               auto ret1 = m_workerLoop.PostTask(std::move(task));
                if (ret1)
                {
                    ret = m_es.PostTask(std::move(task));
                }
            }
            else if (c == ThreadType::Compute)
            {
                ret = m_workerLoop.PostTask(std::move(task));
            }
            else
            {
                ret = m_es.PostTask(std::move(task));
            }
        }

    }, config.m_config);
    return ret;
}

int nx::Threadpool::CurrentThreadToken() noexcept
{
    return tCurrentThreadToken;
}

nx::ThreadpoolScheduler nx::Threadpool::get_scheduler(PostConfig config) noexcept
{
    return nx::ThreadpoolScheduler{this, config};
}

nx::Timer nx::Threadpool::MakeTimer()
{
    return Timer{&m_es};
}


nx::ThreadpoolScheduler::ThreadpoolScheduler(Threadpool* pool, PostConfig threadConf) noexcept
    : m_threadpool(pool), m_conf(threadConf)
{
}

nx::ThreadpoolSender nx::ThreadpoolScheduler::schedule() const noexcept
{
    return {m_threadpool, m_conf};
}

nx::ThreadpoolEnv::ThreadpoolEnv(Threadpool* threadpool, PostConfig threadToken) noexcept
    : m_threadpool(threadpool), m_conf(threadToken)
{
}


nx::ThreadpoolSender::ThreadpoolSender(Threadpool* pool, PostConfig threadToken) noexcept
    : m_threadpool(pool), m_conf(threadToken)
{
}

nx::ThreadpoolEnv nx::ThreadpoolSender::get_env() const noexcept
{
    return {m_threadpool, m_conf};
}

nx::TimeoutSender::TimeoutSender(Threadpool* pool, Timer::Duration timeout) noexcept
    : m_threadpool(pool), m_timeout(timeout)
{
}

nx::TimeoutSender nx::timeout(Threadpool& pool, Timer::Duration timeout)
{
    return {&pool, timeout};
}
