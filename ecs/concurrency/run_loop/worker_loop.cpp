module;
#include <thread>
#include <format>
#include "log/log.h"
#include "container/concurrent_queue.h"
module nx.concurrency.run_loop.worker_loop;
import nx.core.log;
import nx.concurrency.utils.pause;
import nx.core.exception;

LOGGER(worker_loop);


thread_local nx::ThreadPack * tThreadPack = nullptr;

constexpr size_t GlobalTaskQueueSize = 1024;
constexpr size_t LocalTaskQueueSize = 128;
constexpr size_t MaxSpinCount = 20;


nx::WorkerLoop::WorkerLoop(size_t threadSize, std::pmr::memory_resource* resource)
    : m_threads(resource), m_globalTaskQueue(GlobalTaskQueueSize),m_resource(resource),m_threadSize(threadSize)
{
    m_shutdown = false;
    m_threads.reserve(m_threadSize);
    for (int i = 0; i < m_threadSize; ++i)
    {
        m_threads.emplace_back(LocalTaskQueueSize,m_threadSize, m_resource);
    }
}




void nx::WorkerLoop::Run(int threadId)
{
    auto& currentPack = m_threads[threadId];
    tThreadPack = &currentPack;
    auto& localMutex = currentPack.mutex;
    auto& localCv = currentPack.cv;
    auto& waiting = currentPack.waiting;

    auto runTask = [](Task& t)
    {
        try
        {
            t();
        }
        catch (std::exception& e)
        {
            _logger.Error(std::format("uncaught exception thrown in task: {}", e.what()));
        }
        catch (...)
        {
            _logger.Error(std::format("uncaught exception thrown in task!"));
        }
    };
    size_t spinCount = 0;
    while (!m_shutdown)
    {
        if (Task task; currentPack.Consume(task))
        {
            runTask(task);
            spinCount = 0;
            continue;
        }


        if (Task t; m_globalTaskQueue.try_dequeue(t))
        {
            runTask(t);
            spinCount = 0;
            continue;
        }

        auto steal = [&]()
        {
            for (auto& q : m_threads)
            {
                if (&q == &currentPack)continue;
                if (Task task; q.Steal(task, threadId))
                {
                    runTask(task);
                    spinCount = 0;
                    return true;
                }
            }
            return false;
        };

        if (steal())
        {
            continue;
        }

        spinCount++;
        if (spinCount >= MaxSpinCount / 2)
        {
            std::this_thread::yield();
        }
        else if (spinCount >= MaxSpinCount)
        {
            std::unique_lock lock(localMutex);
            waiting.store(true, std::memory_order::acquire);
            localCv.wait(lock, [&]()
            {
                return currentPack.Empty() && !m_globalTaskQueue.size_approx();
            });
            waiting.store(false, std::memory_order::acquire);
        }
        __Pause();
    }
}

void nx::WorkerLoop::Shutdown()
{
    if (m_shutdown)return;

    m_shutdown = true;
    for (auto& t : m_threads)
    {
        t.cv.notify_one();
    }

}

 std::optional<nx::NxError> nx::WorkerLoop::PostTask(Task&& task, int threadId) noexcept
{
    ThreadPack* localPack = tThreadPack;
    if (threadId >= 0 && threadId < m_threads.size())
    {
        localPack = &m_threads[threadId];
    }

    if (localPack)
    {
        if (localPack->Product(std::move(task)))
            return std::nullopt;
    }

    if (m_globalTaskQueue.enqueue(std::move(task)))
        return std::nullopt;

    return  NxError("task post failed");
}

nx::WorkerLoop::~WorkerLoop()
{
    WorkerLoop::Shutdown();
}
