#include "worker_loop.h"
#include <thread>
#include <format>
#include <core/log/log.h>
#include "core//error_code.h"
#include "concurrency/utils/pause.h"
LOGGER(worker_loop);



nx::detail::ThreadPack::ThreadPack(std::pmr::memory_resource* resource, size_t capacity, TaskQueue& globalQueue)
	: m_localQueue(capacity, resource), m_globalProducerToken(globalQueue), m_globalConsumerToken(globalQueue)
{
}

bool nx::detail::ThreadPack::Empty() const noexcept
{
    return m_localQueue.Empty();
}

bool nx::detail::ThreadPack::Produce(Task* task) noexcept
{
	return m_localQueue.Push(task);
}

nx::Task* nx::detail::ThreadPack::Consume() noexcept
{
    return m_localQueue.Pop();
}
nx::Task* nx::detail::ThreadPack::Steal() noexcept
{
    return m_localQueue.Steal();
}

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
        m_threads.emplace_back(m_resource, LocalTaskQueueSize, m_globalTaskQueue);
    }
}
nx::detail::ThreadPack::ThreadPack(ThreadPack&& other) noexcept
    : m_localQueue(std::move(other.m_localQueue)),
      m_globalProducerToken(std::move(other.m_globalProducerToken)),
	m_globalConsumerToken(std::move(other.m_globalConsumerToken))
{

}

nx::detail::ThreadPack& nx::detail::ThreadPack::operator=(ThreadPack&& other) noexcept
{
    if (this != &other) {
        std::exchange(m_localQueue, std::move(other.m_localQueue));
        std::exchange(m_globalProducerToken, std::move(other.m_globalProducerToken));
        std::exchange(m_globalConsumerToken, std::move(other.m_globalConsumerToken));
    }
	return *this;
}



void nx::WorkerLoop::Run(int threadId)
{
    GetCurrentThreadIndex() = threadId;
    auto& currentPack = m_threads[threadId];
    auto& localMutex = currentPack.mutex;
    auto& localCv = currentPack.cv;
    size_t spinCount = 0;

    auto runTask = [&](Task* t)
    {
        try
        {
            spinCount = 0;
            (*t)();
			t->Destroy();
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
        if (Task* task = currentPack.Consume(); task)
        {
            runTask(task);
            continue;
        }


        if (Task* task; m_globalTaskQueue.try_dequeue(task))
        {
            runTask(task);
            continue;
        }

        auto steal = [&]()
        {
            for (auto& q : m_threads)
            {
                if (&q == &currentPack)continue;
                if (Task*  task = q.Steal(); task)
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
        if (spinCount >= (MaxSpinTimes / 2) && spinCount < MaxSpinTimes)
        {
            std::this_thread::yield();
        }
        else if (spinCount >= MaxSpinTimes)
        {
            std::unique_lock lock(localMutex);
            localCv.wait(lock, [&]()
            {
                return !(currentPack.Empty() && m_globalTaskQueue.size_approx() == 0);
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

nx::Error nx::WorkerLoop::PostTask(Task* task, int threadId) noexcept
{
	auto currentThreadId = GetCurrentThreadIndex();
    if (currentThreadId > -1 && currentThreadId < m_threads.size()) {
        if (m_threads[currentThreadId].Produce(task)) {
			return Succeeded;
        }
    }
    if (m_globalTaskQueue.enqueue(task)) {
		return Succeeded;
    }
    return nx::Unexpected( nx::make_error_code(nx::EcsErrc::QueueFull));
}

nx::WorkerLoop::~WorkerLoop()
{
    WorkerLoop::Shutdown();
}

int& nx::WorkerLoop::GetCurrentThreadIndex() noexcept
{
    thread_local  int id = -1;
    return id;
}
