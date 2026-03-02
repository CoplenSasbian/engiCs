module;
#include <functional>
#include "stdexec/execution.hpp"
export module nx.concurrency.threadpool;
import nx.core.types;
import nx.concurrency.error_code;
import nx.core.memory;
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

        Async = -2,
        Compute = -3,
    };

    export
    struct PostConfig
    {
        using TheadToken = int;
        using ConfigType = int;

        [[nodiscard]] constexpr static PostConfig thread(TheadToken token) noexcept
        {
            return PostConfig{token};
        }


        [[nodiscard]] constexpr static PostConfig group(ThreadType type) noexcept
        {
            return PostConfig{type};
        }

        [[nodiscard]] constexpr bool is_thread_token() const noexcept
        {
            return m_config > -1;
        }

        [[nodiscard]] constexpr bool is_thread_group() const noexcept
        {
            return m_config < -1;
        }

        [[nodiscard]] constexpr bool is_any() const noexcept
        {
            return m_config == -1;
        }

       [[nodiscard]] constexpr  Result<ThreadType> get_thread_type() const noexcept
        {
           if (is_thread_token())return  UnExpected(std::make_error_code(std::errc::invalid_argument));
            return static_cast<ThreadType>(m_config);
        }

       [[nodiscard]] constexpr Result<TheadToken> get_thread_token() const noexcept
        {
            if (is_thread_group())
            {
                return UnExpected(std::make_error_code(std::errc::invalid_argument));
            }
            return m_config;
        }

        [[nodiscard]] ThreadType get_thread_type_uncheck()const noexcept
        {
             return static_cast<ThreadType>(m_config);
        }
        [[nodiscard]] TheadToken get_thread_token_uncheck()const noexcept
        {
            return m_config;
        }

        [[nodiscard]] constexpr ConfigType  raw() const noexcept
        {
            return m_config;
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
        ~Threadpool();
        void Shutdown();

        Error PostTask(Task&& task, PostConfig config = AnyThread) noexcept;

        static [[nodiscard]] TheadToken CurrentThreadToken() noexcept;

        [[nodiscard]] TheadToken GetAnyThreadToken(ThreadType = Any) const noexcept;

        [[nodiscard]] ThreadpoolScheduler get_scheduler(PostConfig = AnyThread) noexcept;

        [[nodiscard]] Timer MakeTimer();

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
            if (auto ex = m_threadpool->PostTask([this] { std::execution::set_value(std::move(m_receiver)); }, m_conf); ex)
            {
                auto ePtr = std::make_exception_ptr(std::system_error(ex.value()));
                std::execution::set_error(std::move(m_receiver), std::move(ePtr));
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
        bool operator==(const ThreadpoolSender& rhs) const noexcept ;

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

    export
    template <std::execution::sender S>
    auto StartsOnThreadPool(Threadpool& pool, S&& task, PostConfig threadToken = Threadpool::AnyThread)
    {
        auto sched = pool.get_scheduler(threadToken);
        return std::execution::starts_on(std::move(sched), std::forward<S>(task));
    }

    export
    template <std::execution::sender S>
    auto BindThreadStartsOnThreadPool(Threadpool& pool, S&& task, PostConfig threadToken = Threadpool::AnyThread)
    {

        if (threadToken.is_any())
        {
            int any_thread_token = pool.GetAnyThreadToken();
            return StartsOnThreadPool( pool,std::forward<S>(task),PostConfig::thread(any_thread_token));
        }

        if (threadToken.is_thread_group())
        {
            auto type = threadToken.get_thread_type().value();
            int anyThread = pool.GetAnyThreadToken(type);
            return StartsOnThreadPool( pool,std::forward<S>(task),PostConfig::thread(anyThread));
        }

        auto threadId = threadToken.get_thread_token().value();
        return StartsOnThreadPool( pool,std::forward<S>(task),PostConfig::thread(threadId));

    }




    /********************************************timer***********************************************/


    template <std::execution::receiver R>
    class TimeoutOperationState
    {
    public:
        TimeoutOperationState(R&& receiver, Threadpool* pool, Timer::Duration timeout)
            : m_receiver(std::move(receiver)), m_threadpool(pool), m_timeout(timeout),
              m_timer(m_threadpool->MakeTimer())
        {
        }


        void start() noexcept
        {
            m_timer.SetCallback([this]
            {
                std::execution::set_value(std::move(m_receiver));
            });
            auto ex = m_timer.Start(m_timeout);
            if (ex)
            {
                auto ePtr = std::make_exception_ptr(std::system_error(ex.value()));
                std::execution::set_error(std::move(m_receiver), std::move(ePtr));
            }
        }

    private:
        R m_receiver;
        Threadpool* m_threadpool;
        Timer::Duration m_timeout{};
        Timer m_timer;
    };

    class TimeoutSender : public std::execution::sender_t

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
    TimeoutSender timeout(Threadpool& pool, Timer::Duration timeout);
}
