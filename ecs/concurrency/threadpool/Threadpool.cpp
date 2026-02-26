module;
#include <atomic>
#include <chrono>
#include <thread>
#include <latch>
#include "core/log/log.h"
#include "core/container/concurrent_queue.h"
#include "exec/env.hpp"

module nx.concurrency.threadpool;
import nx.core.exception;
import nx.concurrency.utils.pause;
import nx.core.log;
import nx.core.memory;
LOGGER(thread_pool);


constexpr size_t GlobalTaskQueueSize = 1024;
constexpr size_t LocalTaskQueueSize = 128;
constexpr size_t MaxSpinCount = 20;

thread_local nx::ThreadPack *tCurrentPack;
thread_local int tCurrentThreadToken = -1;


nx::Threadpool::Threadpool(size_t threadNums, std::optional<std::function<void(size_t index)> > cb,
                           std::pmr::memory_resource *resource)
    : m_resource(resource), m_globalTaskQueue(GlobalTaskQueueSize), m_threads(resource) {
    m_threads.reserve(threadNums);
    std::latch latch(threadNums);

    for (size_t i = 0; i < threadNums; i++) {
        m_threads.emplace_back(std::jthread{}, LocalTaskQueueSize, threadNums);
    }

    for (size_t i = 0; i < threadNums; i++) {
        m_threads[i].thread = std::jthread{
            [this,i,&cb,&latch](std::stop_token token) {
                if (cb) cb->operator()(i);
                latch.count_down();
                tCurrentThreadToken = i;
                this->ThreadEntryPoint(token, i);
            }
        };
    }

    latch.wait();
}


nx::Threadpool::~Threadpool() {
    for (auto &q: m_threads) {
        if (q.waiting.load()) {
            q.cv.notify_one();
        }
        q.thread.request_stop();
    }
}

void nx::Threadpool::PostTask(Task &&task,int threadToken ) {
    if (threadToken > -1 && threadToken<m_threads.size()) {
        if (m_threads[threadToken].queue.enqueue(std::move(task))) {
            return;
        }
    }


    if (tCurrentPack) {
        if (tCurrentPack->Product(std::move(task))) {
            tCurrentPack->cv.notify_one();
            return;
        }
    }

    if (m_globalTaskQueue.try_enqueue(std::move(task))) {
        for (auto &q: m_threads) {
            if (q.waiting.load()) {
                q.cv.notify_one();
                break;
            }
        }
        return;
    }
    throw nx::NxError("Task queue is full");
}

int nx::Threadpool::CurrentThreadToken() const noexcept {
    return tCurrentThreadToken;
}

nx::ThreadpoolScheduler nx::Threadpool::get_scheduler() noexcept {
    return nx::ThreadpoolScheduler{this};
}


void nx::Threadpool::ThreadEntryPoint(std::stop_token &token, size_t threadId) {
    auto &currentPack = m_threads[threadId];
    tCurrentPack = &currentPack;
    auto &localMutex = currentPack.mutex;
    auto &localCv = currentPack.cv;
    auto &waiting = currentPack.waiting;

    static auto runTask = [](Task &t) {
        try {
            t();
        } catch (std::exception e) {
            _logger.Error(std::format("uncaught exception thrown in task: {}", e.what()));
        }
    };
    size_t spinCount = 0;
    while (!token.stop_requested()) {
        if (Task task; currentPack.Consume(task)) {
            runTask(task);
            spinCount = 0;
            continue;
        }


        if (Task t; m_globalTaskQueue.try_dequeue(t)) {
            runTask(t);
            spinCount = 0;
            continue;
        }

        auto steal = [&]() {
            for (auto &q: m_threads) {
                if (&q == &currentPack)continue;
                if (Task task; q.Steal(task, threadId)) {
                    runTask(task);
                    spinCount = 0;
                    return true;
                }
            }
            return false;
        };

        if (steal()) {
            continue;
        }

        spinCount++;
        if (spinCount >= MaxSpinCount / 2) {
            std::this_thread::yield();
        } else if (spinCount >= MaxSpinCount) {
            std::unique_lock lock(localMutex);
            waiting.store(true, std::memory_order::acquire);
            localCv.wait(lock, [&]() {
                return currentPack.Empty() && !m_globalTaskQueue.size_approx();
            });
            waiting.store(false, std::memory_order::acquire);
        }
        __Pause();
    }
}

nx::ThreadpoolEvn::ThreadpoolEvn(Threadpool *threadpool) noexcept
:m_threadpool(threadpool){

}

nx::ThreadpoolSender::ThreadpoolSender(Threadpool *threadpool,int threadToken) noexcept
: m_threadpool{threadpool},m_threadToken {threadToken} {}

auto nx::ThreadpoolSender::get_env() const noexcept {
    return ThreadpoolEvn{m_threadpool};
}

nx::ThreadpoolScheduler::ThreadpoolScheduler(Threadpool *threadpool)
    :m_threadpool(threadpool){
}

nx::ThreadpoolSender nx::ThreadpoolScheduler::schedule() noexcept {
    return nx::ThreadpoolSender{m_threadpool};
}

nx::ThreadpoolSender nx::ThreadpoolScheduler::schedule(int threadToken) noexcept {
    return nx::ThreadpoolSender{m_threadpool,threadToken};
}
