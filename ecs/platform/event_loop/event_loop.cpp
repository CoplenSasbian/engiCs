module;
#include <boost/container/flat_map.hpp>
#include <typeindex>


module nx.platform.event_loop;
import nx.core.utils.soo_any;





struct nx::IEventloop::Impl {
    boost::container::flat_map<std::type_index,Any> m_converterStorge;
    boost::container::flat_multimap<int, IEventScheduler*> m_converterPtrs;
};


nx::IEventloop::IEventloop()
    : m_impl(m_res.allocate<Impl>())
{
}

bool nx::IEventloop::dispatch(void* raw_event_data, int msg) noexcept
{
    const auto pair = m_impl->m_converterPtrs.equal_range(msg);
    bool ret = true;
    if (pair.first != pair.second)
    {
        for (auto i = pair.first; i != pair.second; ++i)
        {
            ret = i->second->schedule_dispatch(raw_event_data);
        }
    }
    return ret;
}

void* nx::IEventloop::_get_scheduler(std::type_index type) const noexcept
{
    if (const auto pair = m_impl->m_converterStorge.find(type);
        pair != m_impl->m_converterStorge.end())
    {
        return pair->second.get();
    }
    return nullptr;
}

void* nx::IEventloop::_add_scheduler(std::type_index  type,int message, Any&& scheduler) noexcept
{
    auto pair = m_impl->m_converterStorge.emplace(type, std::move(scheduler));
    IEventScheduler * scheduler_ptr = static_cast<IEventScheduler*>(pair.first->second.get());
    m_impl->m_converterPtrs.emplace(message,scheduler_ptr);
    return scheduler_ptr;
}





