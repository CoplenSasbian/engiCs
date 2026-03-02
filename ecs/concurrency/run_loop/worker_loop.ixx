module;
#include <functional>
#include <condition_variable>
#include "container/concurrent_queue.h"
export module nx.concurrency.run_loop.worker_loop;
import nx.core.types;

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


        bool Product(Task&& task)
        {
            return queue.enqueue(producerToken, std::move(task));
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
        std::atomic_bool waiting;
    };



    export
    class WorkerLoop : public NoneCopyable
    {
    public:
        WorkerLoop(size_t threadSize,std::pmr::memory_resource* resource);
        void Run(int threadId);
        void Shutdown() ;

         std::optional<NxError> PostTask(Task&& task,int threadId = -1) noexcept;

        ~WorkerLoop() ;

    private:
        std::pmr::vector<ThreadPack> m_threads;
        TaskQueue m_globalTaskQueue;
        std::pmr::memory_resource* m_resource;
        bool m_shutdown = false;
        size_t m_threadSize;
    };

}
