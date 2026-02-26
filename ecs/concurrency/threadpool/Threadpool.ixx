module;
#include <functional>
#include <thread>
#include <condition_variable>
#include "core/container/concurrent_queue.h"
#include "stdexec/execution.hpp"

export module nx.concurrency.threadpool;
import nx.core.memory;
import nx.core.types;

namespace std::execution {
    using namespace stdexec;
}

namespace nx {
    using Task = std::function<void()>;
    using TaskQueue = moodycamel::ConcurrentQueue<Task>;


    struct ThreadPack : NoneCopyable {
        template<class ThEp>
        ThreadPack(ThEp &&ep, std::size_t queueSize, size_t totalThreadSize)
            : totalThreadSize(totalThreadSize),
              thread(std::forward<ThEp>(ep)),
              queue(queueSize),
              producerToken(queue) {
            consumerTokens.reserve(totalThreadSize);
            for (std::size_t index = 0; index < totalThreadSize; index++) {
                consumerTokens.emplace_back(queue);
            }
        }

        ThreadPack(ThreadPack &&other) noexcept
            : totalThreadSize(other.totalThreadSize),
              thread(std::move(other.thread)),
              queue(std::move(other.queue)),
              consumerTokens(std::move(other.consumerTokens)) {
        }


        bool Product(Task &&task) {
            return queue.enqueue(producerToken, std::move(task));
        }

        bool Consume(Task &task) {
            return queue.try_dequeue_from_producer(producerToken, task);
        }

        bool Steal(Task &task, size_t threadId) {
            return queue.try_dequeue(consumerTokens[threadId], task);
        }

        bool Empty() const {
            return !queue.size_approx();
        }

        size_t totalThreadSize;
        TaskQueue queue;
        std::jthread thread;
        TaskQueue::producer_token_t producerToken{queue};

        std::vector<TaskQueue::consumer_token_t> consumerTokens{};
        std::mutex mutex;
        std::condition_variable cv;
        std::atomic_bool waiting;
    };


    export
    class ThreadpoolScheduler;

    export
    enum ThreadType {
        Unknown = -1,
        Any = Unknown,

        Async = 0,
        Compute = 1,
    };


    export
    struct Threadpool : NoneCopyable {
        explicit Threadpool(std::size_t threadNums,
                            std::optional<std::function<void(std::size_t index)> > = std::nullopt,
                            std::pmr::memory_resource *resource = std::pmr::get_default_resource());

        Threadpool(Threadpool &&) noexcept = default;

        ~Threadpool();

        void PostTask(Task &&task, int threadToken = -1);
        void PostTask(Task &&task, ThreadType type = Async);

        [[nodiscard]] int CurrentThreadToken() const noexcept;

        [[nodiscard]] ThreadpoolScheduler get_scheduler() noexcept;


        struct SchedulerAwaitable {
            Threadpool *m_threadpool;

            SchedulerAwaitable(Threadpool *pool) : m_threadpool(pool) {
            }

            bool await_ready() const noexcept {
                return false;
            }

            void await_suspend(std::coroutine_handle<>h) const noexcept {
                m_threadpool->PostTask([h] {
                    h.resume();
                });
            }
            void await_resume() const noexcept {
            }

        };

        [[nodiscard]] SchedulerAwaitable AwaitScheduler() {
            return SchedulerAwaitable{this};
        }

    private:
        void ThreadEntryPoint(std::stop_token &, size_t);

        std::pmr::memory_resource *m_resource;
        TaskQueue m_globalTaskQueue;

        std::pmr::vector<ThreadPack> m_threads;
    };

    /*********************************** execution ********************************************/

    export
    class ThreadpoolScheduler;

    export
    class GetCurrentThreadTokenT {
    };

    export
    constexpr GetCurrentThreadTokenT GetCurrentThreadToken{};

    export
    template<std::execution::receiver R>
    class ThreadpoolOp {
    public:
        explicit ThreadpoolOp(Threadpool *threadpool, std::add_rvalue_reference_t<R> r, int threadToken) noexcept
            : m_receiver(std::move(r)), m_threadpool{threadpool}, m_threadToken(threadToken) {
        }

        ThreadpoolOp(ThreadpoolOp &&) noexcept = default;

        void start() noexcept {
            try {
                if (std::execution::get_stop_token(std::execution::get_env(m_receiver)).stop_requested()) {
                    std::execution::set_stopped(std::move(m_receiver));
                    return;
                }
                m_threadpool->PostTask([this] {
                    std::execution::set_value(std::move(m_receiver));
                }, m_threadToken);
            } catch (...) {
                std::execution::set_error(std::move(m_receiver), std::current_exception());
            }
        }

    private:
        R m_receiver;
        Threadpool *m_threadpool;
        int m_threadToken;
    };

    export
    class ThreadpoolEvn {
    public:
        ThreadpoolEvn(Threadpool *threadpool) noexcept;

        friend ThreadpoolScheduler tag_invoke(std::execution::get_scheduler_t, const ThreadpoolEvn &);

        friend int tag_invoke(GetCurrentThreadTokenT, const ThreadpoolEvn &self);

    private:
        Threadpool *m_threadpool;
    };


    export
    class ThreadpoolSender :public std::execution::sender_t{
    public:
        using is_sender = void;

        explicit ThreadpoolSender(Threadpool *threadpool, int threadToken = -1) noexcept;

        ThreadpoolSender(ThreadpoolSender &&) noexcept = default;

        ThreadpoolSender(const ThreadpoolSender &) noexcept = default;
        ThreadpoolSender &operator=(ThreadpoolSender &&) noexcept = default;

        ~ThreadpoolSender() = default;


        bool operator==(const ThreadpoolSender &other) const noexcept {
            return m_threadpool == other.m_threadpool;
        }

        auto constexpr  get_completion_signatures(const auto& evn) const noexcept {
            return std::execution::completion_signatures<
                std::execution::set_value_t(),
                std::execution::set_error_t(std::exception_ptr),
                std::execution::set_stopped_t()
            >{};
        }

        [[nodiscard]] auto get_env() const noexcept;


        template<std::execution::receiver R>
        ThreadpoolOp<R> connect(std::add_rvalue_reference_t<R> r) const noexcept {
            return ThreadpoolOp<R>{m_threadpool, std::move(r), m_threadToken};
        }

    private:
        Threadpool *m_threadpool;
        int m_threadToken;
    };

    export
    class ThreadpoolScheduler {
        Threadpool *m_threadpool;

    public:
        explicit ThreadpoolScheduler(Threadpool *threadpool);

        ThreadpoolScheduler(ThreadpoolScheduler &&) = default;

        ThreadpoolScheduler &operator=(ThreadpoolScheduler &&) = default;

        ThreadpoolScheduler(const ThreadpoolScheduler &) = default;

        ThreadpoolScheduler &operator=(const ThreadpoolScheduler &) = default;

        bool operator==(const ThreadpoolScheduler &other) const {
            return m_threadpool == other.m_threadpool;
        }

         ThreadpoolSender schedule() noexcept;

        ThreadpoolSender schedule(int threadToken) noexcept;

        ~ThreadpoolScheduler() = default;
    };

    ThreadpoolScheduler tag_invoke(std::execution::get_scheduler_t, const ThreadpoolEvn &evn) {
        return ThreadpoolScheduler{evn.m_threadpool};
    }

    int tag_invoke(GetCurrentThreadTokenT, const ThreadpoolEvn &self) {
        return self.m_threadpool->CurrentThreadToken();
    }
}
