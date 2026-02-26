module;
#include <ranges>
#include <atomic>
#include <chrono>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "core/container/concurrent_queue.h"
#endif

module nx.concurrency.utils.event;
import nx.core.memory;
import nx.core.exception;
import nx.concurrency.utils.pause;


std::atomic_uint32_t nx::IEvent::m_eventCounter(0);





enum IOCP_KEY:ULONG_PTR{
    IOCP_EXIT_KEY,IOCP_ASYNC_KEY,IOCP_NEW_TIMER_KEY
};

nx::IEvent::IEvent()
    : m_listeners(MakeShared<ListenerPackVec>()), m_eventToken(++m_eventCounter) {
}

void nx::IEvent::Set() {
    Emit();
}

void nx::IEvent::Emit() const {
    if (m_isCancelled) { return; }
    auto listeners = m_listeners.load();
    for (auto l: *listeners) {
        l.listener();
    }
}


void nx::IEvent::Cancel() {
    m_isCancelled = true;
}

nx::IEvent::Token nx::IEvent::AddListener(Listener listener) {
    ListenerPackVecPtr old, new_;
    Token newToken;
    newToken.hP = ++m_tokenCounter;
    newToken.lP = m_eventToken;

    do {
        old = m_listeners.load(std::memory_order_acquire);
        new_ = MakeShared<ListenerPackVec>();
        new_->reserve(old->size() + 1);
        new_->append_range(*old);
        new_->push_back({std::move(listener), newToken});
    } while (!m_listeners.compare_exchange_strong(old, new_, std::memory_order_release));

    return newToken;
}

void nx::IEvent::RemoveListener(Token token) {
    if (token.lP != m_eventToken) { return; }

    ListenerPackVecPtr old, new_;
    do {
        old = m_listeners.load(std::memory_order_acquire);
        auto foundIt = std::ranges::find_if(*old, [&](auto &c) {
            return c.token == token;
        });
        if (foundIt == old->end()) {
            return;
        }

        new_ = MakeShared<ListenerPackVec>();
        new_->reserve(old->size() - 1);
        new_->insert(new_->begin(), old->begin(), foundIt);
        new_->insert(new_->end(), std::next(foundIt), old->end());
    } while (!m_listeners.compare_exchange_strong(old, new_, std::memory_order_release));
}



nx::AsyncEvent::AsyncEvent(EventSource *es)
    :m_eventSource(es){
}

nx::AsyncEvent::~AsyncEvent() {
}


#ifdef WIN32
struct AsyncOverlapped {
    explicit AsyncOverlapped(std::shared_ptr<nx::AsyncEvent>&& e) : overlapped(), event(e) {
    }
    ~AsyncOverlapped() =default;
    OVERLAPPED overlapped;
    std::shared_ptr<nx::AsyncEvent> event;
};
void nx::AsyncEvent::Set() {
    auto r = Make<AsyncOverlapped>(shared_from_this());
    std::unique_ptr<AsyncOverlapped> raii{r};
    if (PostQueuedCompletionStatus(m_eventSource->m_handle, 0,
        IOCP_ASYNC_KEY,reinterpret_cast<LPOVERLAPPED>(raii.get()))){
        raii.release();
    }
}

#endif

void nx::Timer::Set() {
    Enqueue();
}


nx::Timer::Timer(EventSource *event_source, Clock::duration duration, bool repeat)
    : m_eventSource(event_source),
      m_duration(duration),
      m_repeat(repeat) {
}

void nx::Timer::Enqueue() {
    auto now = Clock::now();
    while (!m_eventSource->m_appendBuffer.try_enqueue({shared_from_this(), now + m_duration })) {
        __Pause();
    }
    auto endTime = Clock::now();
    long long count = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - now).count();
    std::printf("enqueue time %lld ns\n",count);
    m_eventSource->PostNewTimerSignal();
}

#ifdef WIN32
nx::EventSource::EventSource()
    :m_timerQueue(GetCommandResource()){
    m_timerQueue.reserve(2048);
    m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    if (!m_handle) {
        throw NxSystemError("CreateIoCompletionPort failed");
    }
    m_thread = std::jthread([this]() { ThreadEntryPoint(); });
}
#endif


nx::EventSource::~EventSource() {
    Shutdown();
}

#ifdef WIN32
void nx::EventSource::Shutdown() {
    if (m_handle) {
        PostQueuedCompletionStatus(m_handle, 0, IOCP_EXIT_KEY, nullptr);
        m_thread.join();
        CloseHandle(m_handle);
        m_handle = nullptr;
    }
}
#endif


std::shared_ptr<nx::AsyncEvent> nx::EventSource::CreateAsyncEvent() {
    return MakeShared<AsyncEvent>(this);
}

std::shared_ptr<nx::Timer> nx::EventSource::DelayEvent(Timer::Clock::duration delay, bool automaticStart) {
    auto ptr = std::allocate_shared<Timer, std::pmr::polymorphic_allocator<Timer> >(GetCommandResource(),
        this, delay, false
    );
    if (automaticStart) {
        ptr->Set();
    }
    return ptr;
}

std::shared_ptr<nx::Timer> nx::EventSource::RepeatEvent(Timer::Clock::duration delay, bool automaticStart) {
    auto ptr = std::allocate_shared<Timer, std::pmr::polymorphic_allocator<Timer> >(GetCommandResource(),
        this, delay, true
    );
    if (automaticStart) {
        ptr->Set();
    }
    return ptr;
}

#ifdef WIN32


void nx::EventSource::ThreadEntryPoint(){
     static constexpr auto MinWait= std::chrono::milliseconds{16};
    static constexpr  auto RunDelta = std::chrono::nanoseconds{500};
    DWORD dwBytesTransferred;
    ULONG_PTR ulCompletionKey;
    OVERLAPPED *lpOverlapped;
    while (true) {
        if (DoMergeQueue()>0) { // 从无锁队列中获取新的定时任务 ， 返回新任务数
            std::ranges::sort(m_timerQueue,std::greater<>());
        }
        auto wait = MinWait.count();
        while (!m_timerQueue.empty()){
            auto& top = m_timerQueue.back();
            auto deltaTime = top. tp - Timer::Clock::now();;
            if ( deltaTime < RunDelta) {
                top.timer->Emit();
                if (top.timer->IsRepeat()) {
                    top.timer->Enqueue();
                }
                m_timerQueue.pop_back();

            }else if (deltaTime < MinWait) {
                __Pause();
            }else {
                wait = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime -  MinWait).count();
                break;
            }
        }

        m_isBlocking.store(true);
        BOOL bRet = GetQueuedCompletionStatus(
            m_handle,
            &dwBytesTransferred,
            &ulCompletionKey,
            &lpOverlapped,
            wait);
        m_isBlocking.store(false);


        DWORD errorCode = ERROR_SUCCESS;
        if (!bRet) {
            errorCode = GetLastError();
            if (errorCode == WAIT_TIMEOUT) {
                continue;
            }
        }
        switch (ulCompletionKey) {
            case IOCP_ASYNC_KEY:  // 普通异步任务
                if (lpOverlapped)[[likely]] {
                    auto asyncOverlapped = reinterpret_cast<AsyncOverlapped*>(lpOverlapped);
                    asyncOverlapped->event->Emit();
                    Destroy(asyncOverlapped);
                }
                continue;
            case IOCP_EXIT_KEY:[[unlikely]] // 退出信号
                return;
            case IOCP_NEW_TIMER_KEY:
                continue;
        }


    }
}

void nx::EventSource::PostNewTimerSignal() {

#ifdef WIN32
    if (m_isBlocking)
        PostQueuedCompletionStatus(m_handle, 0,IOCP_NEW_TIMER_KEY,nullptr);
#endif
}


#endif

size_t nx::EventSource::DoMergeQueue() {
    auto maxSize = m_timerQueue.capacity() - m_timerQueue.size();
    size_t insertSize = 0;
    size_t size = 0;
    do {
         size =  m_appendBuffer.try_dequeue_bulk(std::back_inserter(m_timerQueue),maxSize);
        insertSize += size;
    }while (size == maxSize);

    return insertSize;
}
