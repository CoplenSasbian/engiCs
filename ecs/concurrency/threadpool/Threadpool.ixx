module;
#include <functional>


#include "stdexec/execution.hpp"


export module nx.concurrency.threadpool;
import nx.core.memory;
import nx.core.types;
import nx.concurrency.run_loop.worker_loop;
import nx.concurrency.run_loop.event_source;

namespace std::execution
{
    using namespace stdexec;
}

namespace nx
{
    using Task = std::function<void()>;

    export
    struct Threadpool;
    export
    class ThreadpoolScheduler;

    export
    enum ThreadType
    {
        Unknown = -1,
        Any = Unknown,

        Async = 0,
        Compute = 1,
    };


    struct PostConfig
    {
        using TheadToken = int;
        using ConfigType = std::variant<int, ThreadType>;

        constexpr static PostConfig thread(TheadToken token)
        {
            return PostConfig(token);
        }


        constexpr static PostConfig group(ThreadType type)
        {
            return PostConfig(type);
        }


        constexpr PostConfig(PostConfig&&) noexcept = default;
        constexpr PostConfig(const PostConfig&) noexcept = default;


        constexpr bool operator==(const PostConfig& rhs) const = default;

    private:
        friend struct Threadpool;

        constexpr PostConfig(const ConfigType config) : m_config(config)
        {
        }

        ConfigType m_config;
    };


    struct Threadpool : NoneCopyable
    {
        using TheadToken = int;
        using enum ThreadType;


        static constexpr PostConfig ComputeThreads = PostConfig::group(Compute);
        static constexpr PostConfig AsyncThreads = PostConfig::group(Async);
        static constexpr PostConfig AnyThread = PostConfig::thread(-1);

        explicit Threadpool(int threadNums,
                            std::optional<std::function<void(std::size_t index)>> = std::nullopt,
                            std::pmr::memory_resource* resource = std::pmr::get_default_resource());

        Threadpool(Threadpool&&) noexcept = delete;
        ~Threadpool() ;
        void Shutdown() ;

        std::optional<NxError> PostTask(Task&& task, PostConfig config = AnyThread) noexcept;

        static [[nodiscard]] int CurrentThreadToken() noexcept;

        [[nodiscard]] ThreadpoolScheduler get_scheduler(PostConfig = AnyThread) noexcept;

        [[nodiscard]] Timer MakeTimer();

        struct SchedulerAwaitable
        {
            Threadpool* m_threadpool;

            SchedulerAwaitable(Threadpool* pool) : m_threadpool(pool)
            {
            }

            bool await_ready() const noexcept
            {
                return false;
            }

            void await_suspend(std::coroutine_handle<> h) const noexcept
            {
                m_threadpool->PostTask([h]
                {
                    h.resume();
                });
            }

            void await_resume() const noexcept
            {
            }
        };

        [[nodiscard]] SchedulerAwaitable AwaitScheduler()
        {
            return SchedulerAwaitable{this};
        }

    private:
        std::pmr::memory_resource* m_resource;
        WorkerLoop m_workerLoop;
        std::pmr::vector<std::jthread> m_workerThreads;
        size_t m_threadNums;
        EventLoop m_es;
        std::jthread m_esThread;
        std::optional<std::function<void(size_t index)>> m_threadInit;
    };


    /*********************************** execution ********************************************/

    export class ThreadpoolEnv;
    export class ThreadpoolSender;
    export
    template <std::execution::receiver R>
    class ThreadpoolOperationState;


    class ThreadpoolScheduler
    {
    public:
        ThreadpoolScheduler(Threadpool* pool, PostConfig threadToken) noexcept;

        ThreadpoolScheduler(const ThreadpoolScheduler&) noexcept = default;
        ThreadpoolScheduler(ThreadpoolScheduler&&) noexcept = default;
        [[nodiscard]] ThreadpoolSender schedule() const noexcept;

        bool operator==(const ThreadpoolScheduler& rhs) const noexcept = default;

    private:
        Threadpool* m_threadpool;
        PostConfig m_conf;
    };


    class ThreadpoolEnv
    {
    public:
        ThreadpoolEnv(Threadpool* threadpool, PostConfig threadToken) noexcept;
        ThreadpoolEnv(ThreadpoolEnv&&) noexcept = default;
        ThreadpoolEnv(const ThreadpoolEnv&) noexcept = default;

    private:
        Threadpool* m_threadpool;
        PostConfig m_conf;
    };


    template <std::execution::receiver R>
    class ThreadpoolOperationState
    {
    public:
        ThreadpoolOperationState(R&& receiver, Threadpool* pool, PostConfig threadToken) noexcept
            : m_receiver(std::move(receiver)), m_threadpool(pool), m_conf(threadToken)
        {
        };

        void start() noexcept
        {
                if (std::execution::get_stop_token(std::execution::get_env(m_receiver)).stop_requested())
                {
                    std::execution::set_stopped(std::move(m_receiver));
                    return;
                }
                if (auto ex = m_threadpool->PostTask([this]{std::execution::set_value(std::move(m_receiver));});ex)
                {
                    std::execution::set_error(std::move(m_receiver), std::current_exception());
                }
        }

    private:
        R m_receiver;
        Threadpool* m_threadpool;
        PostConfig m_conf;
    };


    class ThreadpoolSender : public std::execution::sender_t
    {
    public:
        ThreadpoolSender(Threadpool* pool, PostConfig threadToken) noexcept;
        ThreadpoolSender(const ThreadpoolSender&) noexcept = default;
        ThreadpoolSender(ThreadpoolSender&&) noexcept = default;
        bool operator==(const ThreadpoolSender& rhs) const noexcept = default;

        template <std::execution::receiver R>
        ThreadpoolOperationState<R> connect(R&& receiver) noexcept
        {
            return ThreadpoolOperationState<R>{std::forward<R>(receiver), m_threadpool, m_conf};
        }

        [[nodiscard]] ThreadpoolEnv get_env() const noexcept;

        static auto get_completion_signatures(const auto& env) noexcept
        {
            return std::execution::completion_signatures<
                std::execution::set_error_t(std::exception_ptr),
                std::execution::set_value_t(),
                std::execution::set_stopped_t()
            >{};
        }

    private:
        Threadpool* m_threadpool;
        PostConfig m_conf;
    };

    /********************************************timer***********************************************/
    template<std::execution::receiver R>
    class TimeoutOperationState
    {
    public:
        TimeoutOperationState(R&& receiver, Threadpool* pool, Timer::Duration timeout)
            : m_receiver(std::move(receiver)), m_threadpool(pool), m_timeout(timeout),m_timer(m_threadpool->MakeTimer())
        {}


        void start() noexcept
        {

            m_timer.SetCallback([this]
            {
                std::execution::set_value(std::move(m_receiver));
            });
            auto ex = m_timer.Start(m_timeout);
            if (ex)
            {
                std::execution::set_error(std::move(m_receiver), std::make_exception_ptr(ex.value()));
            }
        }

    private:
        R m_receiver;
        Threadpool* m_threadpool;
        Timer::Duration m_timeout;
        Timer m_timer;
    };

    class TimeoutSender:public std::execution::sender_t

    {
    public:
        TimeoutSender(Threadpool* pool, Timer::Duration timeout) noexcept;

        TimeoutSender(const TimeoutSender&) noexcept = delete;
        TimeoutSender(TimeoutSender&&) noexcept = default;


        auto get_completion_signatures(auto) const noexcept
        {
            return std::execution::completion_signatures<
                std::execution::set_value_t(),
                std::execution::set_error_t(std::exception_ptr)
            >{};
        }

        bool operator==(const TimeoutSender& rhs) const noexcept
        {
            return m_threadpool == rhs.m_threadpool
            && m_timeout == rhs.m_timeout;
        }

        template <std::execution::receiver R>
        TimeoutOperationState<R> connect(R&& receiver) noexcept
        {
            return {std::forward<R>(receiver), m_threadpool, m_timeout};
        }


    private:
        friend struct Awaitable;
        Threadpool* m_threadpool;
        Timer::Duration m_timeout;
    };




    export
    TimeoutSender timeout(Threadpool&  pool,Timer::Duration  timeout);
}
