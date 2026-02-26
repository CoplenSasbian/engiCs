module;
#include <thread>
#include <chrono>
#include <functional>
#include <memory>
#include "core/container/concurrent_queue.h"
export module nx.concurrency.utils.event;
import nx.core.types;
import nx.core.memory;
import nx.concurrency.promise;
namespace nx {
    export
    class EventSource;

    export
    class IEvent {
    public:
        struct Token {
            union {
                struct {
                    std::uint32_t lP;
                    std::uint32_t hP;
                };

                std::uint64_t fullP;
            };

            friend bool operator==(const Token &lhs, const Token &rhs) {
                return lhs.fullP == rhs.fullP;
            };
        };


        using Listener = std::function<void()>;

        IEvent();

        virtual ~IEvent() = default;

        virtual void Cancel();

        virtual void Set();

        bool IsCancelled()const {return m_isCancelled;}
        void Emit() const;

        auto When() {
            return MakeCallbackPromise<>([this](std::function<void()>&& fun) {
                 AddListener(std::move(fun));
            });
        }

        Token AddListener(Listener listener);

        void RemoveListener(Token token);

    private:
        struct ListenerPack {
            Listener listener;
            Token token;
        };

        using ListenerPackVec = std::pmr::vector<ListenerPack>;
        using ListenerPackVecPtr = std::shared_ptr<ListenerPackVec>;
        std::atomic_uint32_t m_tokenCounter = 0;
        std::atomic<ListenerPackVecPtr> m_listeners;
        volatile bool m_isCancelled = false;

        static std::atomic_uint32_t m_eventCounter;
        const uint32_t m_eventToken;
    };


    export class AsyncEvent:public IEvent,public std::enable_shared_from_this<AsyncEvent> {
    public:
        explicit AsyncEvent(EventSource* es);
        void Set() override;
        ~AsyncEvent() override;
    private:
        EventSource* m_eventSource;

    };

    export
    class Timer : public IEvent,public std::enable_shared_from_this<Timer> {
    public:
        using Clock = std::chrono::high_resolution_clock;

        ~Timer() override=default;


        Clock::duration GetDuration() const {return m_duration;}
        void Set() override;
        bool IsRepeat()const {return m_repeat;}

        explicit Timer(EventSource *event_source, Clock::duration duration, bool repeat);



    private:
        void Enqueue();

        friend class EventSource;

        EventSource *m_eventSource;
        Clock::duration m_duration;
        bool m_repeat;
    };

    struct TimerPack : NoneCopyable {
        TimerPack(std::shared_ptr<Timer> timer,
            Timer::Clock::time_point point
        )
            : tp(point),
              timer(timer) {
        }
        TimerPack()=default;

        TimerPack(TimerPack && rhs) noexcept
        : tp(std::move(rhs.tp)), timer(std::move(rhs.timer)){
        }


        TimerPack &operator=(const TimerPack& other) {
            tp = other.tp;
            timer = other.timer;
            return *this;
        }

        Timer::Clock::time_point tp;
        std::shared_ptr<Timer> timer;

        friend bool operator==(const TimerPack &lhs, const TimerPack &rhs) {
            return lhs.timer == rhs.timer;
        }

       friend std::strong_ordering operator<=>(const TimerPack &lhs, const TimerPack &rhs) {
            return lhs.tp<=> rhs.tp;
        }
    };


    class AsyncFile {
    public:
        AsyncFile();
    private:
        void* m_nativeHandle;
    };


    class EventSource : public NonTransferable {
    public:
        EventSource();

        ~EventSource();

        void Shutdown();

        std::shared_ptr<AsyncEvent> CreateAsyncEvent();

        std::shared_ptr<Timer> DelayEvent(Timer::Clock::duration delay, bool automaticStart = true);
        std::shared_ptr<Timer> RepeatEvent(Timer::Clock::duration delay, bool automaticStart = true);

    private:
        void ThreadEntryPoint();
        void PostNewTimerSignal();
        size_t DoMergeQueue();
        moodycamel::ConcurrentQueue<TimerPack> m_appendBuffer;
        std::pmr::vector<TimerPack> m_timerQueue;
        std::jthread m_thread;
        std::atomic_bool m_isBlocking = false;

        void *m_handle;
        friend class Timer;
        friend class AsyncEvent;
    };
}
