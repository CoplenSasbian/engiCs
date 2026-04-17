#include "message_loop.h"
#include <boost/container/flat_map.hpp>
#include <typeindex>
#include "core/error_code.h"
#include "concurrency/container/moodycamel/concurrent_queue.h"
#include "core/log/log.h"
#include <format>
LOGGER(message_loop);
struct nx::MessageLoopBase::Impl {
    boost::container::flat_map<std::type_index, Any> m_converterStorge;
    boost::container::flat_multimap<PlatformMessageType, std::type_index> m_converterIndex;
    moodycamel::ConcurrentQueue<Task*> m_taskQueue;
};


nx::MessageLoopBase::MessageLoopBase(std::pmr::memory_resource* resource) noexcept
    :IMessageLoop(resource), m_impl(m_res.allocate<Impl>())
{
}

nx::Error nx::MessageLoopBase::PostTaskP(Task* task) noexcept
{
    if (!m_impl->m_taskQueue.enqueue(task))
    {
        return nx::Unexpected(nx::make_error_code(EcsErrc::QueueFull));
    }
    return Succeeded;
}



void nx::MessageLoopBase::Run() noexcept
{
    while (PoolEvent())
    {
        for (Task* task;m_impl->m_taskQueue.try_dequeue(task);)
        {
            try
            {
                (*task)();
				task->Destroy();
            }
            catch (const std::exception& e)
            {
				_logger.Error(std::format("Exception occurred while executing task in message loop: {}", e.what()));
            }
            catch (...)
            {
				_logger.Error("Exception occurred while executing task in message loop");
            }

        }
    }
}


bool nx::MessageLoopBase::dispatch(void* raw_event_data, PlatformMessageType msg) noexcept
{
    const auto pair = m_impl->m_converterIndex.equal_range(msg);
    bool ret = true;
    if (pair.first != pair.second)
    {
        for (auto i = pair.first; i != pair.second; ++i)
        {
            auto sche = static_cast<IMessageScheduler*>(m_impl->m_converterStorge[i->second].get());
            ret = sche->schedule_dispatch(raw_event_data) && ret;
        }
    }
    return ret;
}

void* nx::MessageLoopBase::_get_scheduler(std::type_index type) const noexcept
{
    if (const auto pair = m_impl->m_converterStorge.find(type);
        pair != m_impl->m_converterStorge.end())
    {
        return pair->second.get();
    }
    return nullptr;
}

void* nx::MessageLoopBase::_add_scheduler(std::type_index  type,std::span<PlatformMessageType> messages, Any&& scheduler) noexcept
{
    auto pair = m_impl->m_converterStorge.emplace(type, std::move(scheduler));
    IMessageScheduler * scheduler_ptr = static_cast<IMessageScheduler*>(pair.first->second.get());

    for (PlatformMessageType message : messages)
        m_impl->m_converterIndex.emplace(message,type);
    return scheduler_ptr;
}





