#include "worker_loop.h"
#include <thread>
#include <format>
#include <core/log/log.h>
#include "core/container/concurrent_queue.h"
#include "core//error_code.h"
#include "concurrency/utils/pause.h"
LOGGER(worker_loop);



constexpr size_t GlobalTaskQueueSize = 1024;

constexpr size_t LocalTaskQueueSize = 128;
constexpr size_t MaxSpinTimes = 128 ;



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
    GetCurrentThreadIndex() = threadId;
    auto& currentPack = m_threads[threadId];
    auto& localMutex = currentPack.mutex;
    auto& localCv = currentPack.cv;
    size_t spinCount = 0;

    auto runTask = [&](Task& t)
    {
        try
        {
            spinCount = 0;
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
    while (!m_shutdown)
    {
        if (Task task; currentPack.Consume(task))
        {
            runTask(task);
            continue;
        }


        if (Task t; m_globalTaskQueue.try_dequeue(t))
        {
            runTask(t);
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
		//_logger.Trace(std::format("thread {} is spinning, spin count: {}", threadId, spinCount));
        if (spinCount >= (MaxSpinTimes / 2) && spinCount < MaxSpinTimes)
        {
			_logger.Trace(std::format("thread {} is yielding, spin count: {}", threadId, spinCount));
            std::this_thread::yield();
        }
        else if (spinCount >= MaxSpinTimes)
        {
            _logger.Trace(std::format("thread {} is waiting, spin count: {}", threadId, spinCount));

            std::unique_lock lock(localMutex);
            localCv.wait(lock, [&]()
            {
                return currentPack.Empty() && m_globalTaskQueue.size_approx() == 0;
            });
        }
        __Pause();
    }
}

void nx::WorkerLoop::Shutdown()
{
    if (std::exchange(m_shutdown, true))return;

    for (auto& t : m_threads)
    {
        t.cv.notify_one();
    }

}

 nx::Error nx::WorkerLoop::PostTask(Task&& task, int threadId) noexcept
{
    if (threadId >= 0 && threadId < m_threads.size())
    {
        if (GetCurrentThreadIndex() == threadId)
        {
            if (m_threads[threadId].Product
            (std::move(task)))return Succeeded;

        }else
        {
            if (m_threads[threadId].ProductFromOtherThread(std::move(task)))return Succeeded;
        }
    }


    if (m_globalTaskQueue.try_enqueue(std::move(task)))
    {
        for (auto &t : m_threads)
        {
            t.Notify();
        }
        return Succeeded;
    }

    return   nx::Unexpected(nx::make_error_code(EcsErrc::QueueFull));
}

nx::WorkerLoop::~WorkerLoop()
{
    WorkerLoop::Shutdown();
}

int& nx::WorkerLoop::GetCurrentThreadIndex() noexcept
{
    thread_local  int id;
    return id;
}
