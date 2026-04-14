#pragma once
#include "core/types/class_def.h"
#include <functional>
#include <condition_variable>
#include "riften/deque.hpp"

namespace nx
{
    using Task = std::function<void()>;
    using TaskQueue = moodycamel::ConcurrentQueue<Task>;


    struct ThreadPack : NoneCopyable
    {
        ThreadPack(std::size_t queueSize, size_t totalThreadSize, std::pmr::memory_resource* resource)
            : totalThreadSize(totalThreadSize),
              queue(queueSize),
              producerToken(queue),
              consumerTokens(resource)

        {
            consumerTokens.reserve(totalThreadSize);
            for (std::size_t index = 0; index < totalThreadSize; index++)
            {
                consumerTokens.emplace_back(queue);
            }
        }

        ThreadPack(ThreadPack&& other) noexcept
            : totalThreadSize(other.totalThreadSize),
              queue(std::move(other.queue)),
              consumerTokens(std::move(other.consumerTokens))
        {
        }

        void Notify()
        {
            cv.notify_one();
        }


        bool Product(Task&& task)
        {
            auto  success = queue.try_enqueue(producerToken, std::move(task));
            if ( success) Notify();
            return  success;
        }
        bool ProductFromOtherThread(Task&&  task)
        {
            auto  success = queue.try_enqueue(producerToken, std::move(task));
            if ( success) Notify();
            return  success;
        }

        bool Consume(Task& task)
        {
            return queue.try_dequeue_from_producer(producerToken, task);
        }

        bool Steal(Task& task, size_t threadId)
        {

            return queue.try_dequeue(consumerTokens[threadId], task);
        }

        bool Empty() const
        {
            return !queue.size_approx();
        }

        size_t totalThreadSize;
        TaskQueue queue;
        TaskQueue::producer_token_t producerToken{queue};
        std::pmr::vector<TaskQueue::consumer_token_t> consumerTokens{};
        std::mutex mutex;
        std::condition_variable cv;
    };



    class WorkerLoop : public NoneCopyable
    {
    public:
        WorkerLoop(size_t threadSize,std::pmr::memory_resource* resource);
        void Run(int threadId);
        void Shutdown() ;

         nx::Error PostTask(Task&& task,int threadId = -1) noexcept;

        ~WorkerLoop() ;

    private:
        static int& GetCurrentThreadIndex()noexcept;
        std::pmr::vector<ThreadPack> m_threads{};
        TaskQueue m_globalTaskQueue{};
        std::pmr::memory_resource* m_resource;
        bool m_shutdown = false;
        size_t m_threadSize;
    };

}
