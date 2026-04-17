#pragma once
#include "core/types/class_def.h"
#include <condition_variable>
#include "concurrency/container/deque.h"
#include "concurrency/container/moodycamel/concurrent_queue.h"
#include "concurrency/run_loop/task.h"
namespace nx
{
   
    using TaskQueue = moodycamel::ConcurrentQueue<Task*>;
	using LocalQueue = nx::WorkStealDeque<Task*>;
    
    namespace detail
    {

        struct ThreadPack : NoneCopyable
        {
			ThreadPack(std::pmr::memory_resource* resource,size_t capacity, TaskQueue& globalQueue);

			ThreadPack(ThreadPack&&)noexcept;
			ThreadPack& operator=(ThreadPack&&) noexcept;

            bool Empty() const noexcept;
			bool Produce(Task* task) noexcept;
            Task* Consume() noexcept;
			Task* Steal() noexcept;
          
			LocalQueue m_localQueue;
			moodycamel::ProducerToken m_globalProducerToken;
			moodycamel::ConsumerToken m_globalConsumerToken;
        
			// used for condition variable to wake up the thread when new task is posted to global queue or local queue
			std::mutex mutex;
			std::condition_variable cv;
        
        };


    }


    class WorkerLoop : public NoneCopyable
    {
    public:
        WorkerLoop(size_t threadSize,std::pmr::memory_resource* resource);
        void Run(int threadId);
        void Shutdown() ;

         template<TaskCallable Callable>
         nx::Error PostTask(Callable&& task, int threadId = -1) noexcept {
			 auto taskWrapper = MakeTask(std::forward<Callable>(task), m_resource);
			 auto errc= PostTask(taskWrapper, threadId);
			 if (!errc)
             {
				 taskWrapper->Destroy();
             }

			 return errc;
         };
         nx::Error PostTask(Task* task, int threadId) noexcept;

        ~WorkerLoop() ;

    private:

        static int& GetCurrentThreadIndex()noexcept;
        std::pmr::vector<detail::ThreadPack> m_threads;
        TaskQueue m_globalTaskQueue;
        std::pmr::memory_resource* m_resource;
        bool m_shutdown = false;
        size_t m_threadSize;
    };

}
