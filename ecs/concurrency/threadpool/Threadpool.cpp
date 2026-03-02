module;
#include <atomic>
#include <chrono>
#include <thread>
#include <latch>
#include "log/log.h"
#include "container/concurrent_queue.h"
#include "exec/env.hpp"

module nx.concurrency.threadpool;
import nx.concurrency.error_code;
import nx.concurrency.utils.pause;
import nx.core.log;
import nx.core.memory;
LOGGER(thread_pool);


thread_local nx::Threadpool::TheadToken tCurrentThreadToken = -1;


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

nx::Error nx::Threadpool::PostTask(Task&& task, PostConfig config) noexcept
{
    nx::Error ret = NoError;
    if (config.is_any())
    {
        ret = m_workerLoop.PostTask(std::move(task), -1);
        if (ret) ret = m_es.PostTask(std::move(task));
    }
    else if (config.is_thread_token())
    {
        const auto token = config.get_thread_token_uncheck();
        if (token < m_threadNums)
        {
            ret = m_workerLoop.PostTask(std::move(task), token);
        }
        else if (token == static_cast<int>(m_threadNums))
        {
            ret = m_es.PostTask(std::move(task));
        }
        else
        {
            ret = make_error_code(ConcurrencyErrc::InvalidThreadToken);
        }
    }
    else
    {
        auto type = config.get_thread_type_uncheck();
        switch (type)
        {
        case ThreadType::Compute:
            ret = m_workerLoop.PostTask(std::move(task), GetAnyThreadToken(type));
            break;
        case ThreadType::Async:
            ret = m_es.PostTask(std::move(task));
            break;
        default:
            ret = make_error_code(ConcurrencyErrc::InvalidThreadType);
        }
    }

    return ret;
}

int nx::Threadpool::CurrentThreadToken() noexcept
{
    return tCurrentThreadToken;
}

nx::Threadpool::TheadToken nx::Threadpool::GetAnyThreadToken(ThreadType type) const noexcept
{
    static std::atomic<TheadToken> ret = 0;

    switch (type)
    {
    case Any:
        return ret++ % (static_cast<TheadToken>(m_threadNums) + 1);
    case Compute:
        return ret++ % static_cast<TheadToken>(m_threadNums);
    case Async:
        return static_cast<TheadToken>(m_threadNums);
    default:
        return -1;
    }
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

bool nx::ThreadpoolSender::operator==(const ThreadpoolSender& rhs) const noexcept
{
    return m_threadpool == rhs.m_threadpool && m_conf == rhs.m_conf;
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
