module;
#include <functional>
#include <typeindex>
export module nx.platform.event_loop;
import nx.platform;
import nx.core.memory;
import nx.core.utils.soo_any;
import nx.platform.error_code;
namespace nx
{
    export
    class IEventloop;

    export
    template <PlatformMagic platform_type, class Et>
    struct EventConverter;

    export
    template <PlatformMagic platform_type>
    class EventLoop;



    class IEventScheduler
    {
    public:
        virtual ~IEventScheduler() = default;
        virtual bool schedule_dispatch(void* raw_event_data) noexcept= 0;
    };

    template <PlatformMagic Pt, class EventType>
     class EventScheduler final: public IEventScheduler
    {
    public:
        using Listener = std::function<void(const EventType&)>;
        using Handler = std::function<bool(const EventType&)>;
        using Converter = EventConverter<Pt,EventType>;
        bool schedule_dispatch(void* raw_event_data)noexcept override
        {

            bool ret = true;
            EventType event = Converter::fromNative(raw_event_data);
            if ( m_handler)
            {
                ret = m_handler(event);
            }
            for (auto listener : m_listeners) {
                listener(event);
            }
            return ret;
        }
        void AddListener(Listener&&  listener) noexcept
        {
            m_listeners.emplace_back(std::move(listener));
        }

        void SetHandler(Handler&&  handler)
        {
            m_handler = std::move(handler);
        }
    private:
        std::vector<Listener> m_listeners;
        Handler m_handler;
    };


    constexpr static nx::PlatformMagic fakeMagic = make_platform("");
    struct FakeEvent{};

    template<>
            struct EventConverter<fakeMagic,FakeEvent>
    {
        static FakeEvent fromNative(void* raw_event) noexcept
        {return FakeEvent{};}

        static  void toNative(void* raw_event,const FakeEvent& event) noexcept
        {}
    };

    class IEventloop : public IPlatformComponent
    {



        constexpr static std::size_t scheduler_size = sizeof(nx::EventScheduler<fakeMagic, FakeEvent>);
        using Any = nx::SsoAny<scheduler_size>;
    public:
        IEventloop();
        IEventloop(const IEventloop&) = delete;

        constexpr virtual size_t native_msg_size()noexcept =0;
        virtual ~IEventloop() = default;


        virtual void Run() noexcept = 0;
        virtual void Pool() noexcept =0;
        virtual void PostQuit() noexcept =0;

        virtual Error SentMessage(void* rawEvent) noexcept = 0;

        template <class EventType>
        Error Post(const EventType& event) noexcept
        {
            using Converter = EventConverter<platform_type(),EventType>;
            char buffer[native_msg_size()];
            Converter::toNative(buffer,event);
            return SentMessage(buffer);
        }



        template <class Et>
        void On(std::function<void(Et&)>&& listener) noexcept
        {
            using Scheduler = EventScheduler<platform_type(),Et>;
            Scheduler* scheduler = static_cast<Scheduler*>(_get_or_create_scheduler(typeid(Et)));
            scheduler->AddListener(std::move(listener));
        }

        template <class Et>
        void SetHandler(std::function<bool(const Et&)>&& handler)
        {
            using Scheduler = EventScheduler<platform_type(),Et>;
            Scheduler* scheduler = static_cast<Scheduler*>(_get_or_create_scheduler(typeid(Et)));
            scheduler->SetHandler(std::move(handler));
        }

    protected:
        bool dispatch(void* raw_event_data,int msg) noexcept;
    private:
        template <class Et>
        auto _get_or_create_scheduler(std::type_index type) noexcept
        {
            using Scheduler = EventScheduler<platform_type(),Et>;
            using Converter = EventConverter<platform_type(),Et>;
            Scheduler * scheduler = static_cast<Scheduler*>(_get_scheduler(type));
            if (!scheduler)
            {
                scheduler = _add_scheduler(type,Converter::NativeMessgae,Any(Scheduler{}));
            }
        };

        void* _get_scheduler(std::type_index type) const noexcept;
        void* _add_scheduler (std::type_index,int,Any&&) noexcept;

        AlignedStackPmr<1024> m_res;
        struct Impl;
        Impl* m_impl;
    };





}
