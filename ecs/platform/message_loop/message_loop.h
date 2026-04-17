#pragma once

#include <functional>
#include <typeindex>
#include <span>
#include "platform/win32/win32_platform.h"
#include "core/utils/sso_any.h"
#include "core/memory/memory.h"
#include "event_converter.h"
#include "concurrency/run_loop/task.h"
namespace nx
{
    template <PlatformMagic platform>
    class MessageLoop;


    template <PlatformMagic>
    class SchedulerEventloop;



    using PlatformMessageType = unsigned long;


    template <typename T, typename EventT>
    concept ValidEventConverterForType = requires
    {
        { T::nativeMsg() } -> std::same_as<std::span<PlatformMessageType>>;

        { T::fromNative(static_cast<void*>(nullptr)) } -> std::same_as<EventT>;

        { T::toNative(std::declval<const EventT&>(), static_cast<void*>(nullptr)) };
    };


    template <typename T, typename EventT>
    constexpr bool IsValidEventConverterForType = ValidEventConverterForType<T, EventT>;




    class IMessageScheduler
    {
    public:
        virtual ~IMessageScheduler() = default;
        virtual bool schedule_dispatch(void* raw_event_data) noexcept = 0;
    };

    template <PlatformMagic Pt, class EventType>
    class MessageScheduler final : public IMessageScheduler
    {
    public:
        using Listener = std::function<void(const EventType&)>;
        using Handler = std::function<bool(const EventType&)>;

        using Converter = EventConverter<Pt, EventType>;
        static_assert(IsValidEventConverterForType<Converter, EventType>,
                      "\n\n"
                      "================================================\n"
                      "COMPILER ERROR: Missing or Invalid EventConverter!\n"
                      "================================================\n"
                      "Your specialization of 'EventConverter<Pt, EventType>' is missing required static functions.\n"
                      "Please ensure your specialization provides:\n"
                      "- static std::span<PlatformMessageType> nativeMsg();\n"
                      "- static EventType fromNative(void* rawMsg);\n"
                      "- static void toNative(const EventType& event, void* rawMsg);\n"
                      "================================================\n"
                      "\n\n"
        );


        bool schedule_dispatch(void* raw_event_data) noexcept override
        {
            bool ret = true;
            EventType event = Converter::fromNative(raw_event_data);
            if (m_handler)
            {
                ret = m_handler(event);
            }
            for (auto listener : m_listeners)
            {
                listener(event);
            }
            return ret;
        }

        void AddListener(Listener&& listener) noexcept
        {
            m_listeners.emplace_back(std::move(listener));
        }

        void SetHandler(Handler&& handler)
        {
            m_handler = std::move(handler);
        }

    private:
        std::vector<Listener> m_listeners;
        Handler m_handler;
    };


    constexpr static nx::PlatformMagic fakeMagic = make_platform("fak");

    struct FakeEvent
    {
    };

    template <>
    struct EventConverter<fakeMagic, FakeEvent>
    {
        static FakeEvent fromNative(void* raw_event) noexcept
        {
            return FakeEvent{};
        }

        static void toNative(const FakeEvent& event,void* raw_event) noexcept
        {
        }
        static std::span<PlatformMessageType> nativeMsg()
        {
            return {};
        }
    };
    class IMessageLoop{
    public:
		IMessageLoop(std::pmr::memory_resource* resource = std::pmr::get_default_resource()) noexcept : m_memResource(resource) {}

        virtual ~IMessageLoop() = default;
        virtual void Run() = 0;

		template<TaskCallable Callable>
        Error PostTask(Callable&& task) noexcept
        {
            auto taskWrapper = MakeTask(std::forward<Callable>(task), m_memResource);
            auto errc = PostTaskP(taskWrapper);
            if (!errc) {
                taskWrapper->Destroy();
            }
            return errc;
        }

        virtual  Error PostTaskP(Task*) noexcept = 0;
        virtual void PostQuit() noexcept =0;
        virtual bool PoolEvent()noexcept =0;
        virtual Error SentMessage(void* rawEvent) noexcept = 0;

    private:
		std::pmr::memory_resource* m_memResource;

    };

    class MessageLoopBase:public IMessageLoop
    {
        constexpr static std::size_t scheduler_size = sizeof(nx::MessageScheduler<fakeMagic, FakeEvent>);

    public:

        using Any = nx::SsoAny<scheduler_size>;
        MessageLoopBase(std::pmr::memory_resource* resource = std::pmr::get_default_resource()) noexcept;
        ~MessageLoopBase() override = default;
        Error PostTaskP(Task*) noexcept override;
        void Run()noexcept override;
    protected:
        bool dispatch(void* raw_event_data, PlatformMessageType msg) noexcept;
        [[nodiscard]] void* _get_scheduler(std::type_index type) const noexcept;
        void* _add_scheduler(std::type_index, std::span<PlatformMessageType>, Any&&) noexcept;
        AlignedStackPmr<1024> m_res;
        struct Impl;
        Impl* m_impl;
    };



    template <PlatformMagic platform>
    class BaseMessageLoop : public MessageLoopBase
    {
    public:
        BaseMessageLoop() = default;
        BaseMessageLoop(const BaseMessageLoop&) = delete;
        ~BaseMessageLoop() override = default;

        template <class EventType>
        Error Post(const EventType& event) noexcept
        {
            using Converter = EventConverter<platform, EventType>;
            char buffer[256];
            Converter::toNative(event,buffer);
            return SentMessage(buffer);
        }

        template <class Et>
        void On(MessageScheduler<platform, Et>::Listener && listener) noexcept
        {
            auto* scheduler = _get_or_create_scheduler<Et>();
            scheduler->AddListener(std::move(listener));
        }
        template<typename EventType, typename Class>
        void On(void (Class::* mem_func)(const EventType&), Class* obj) noexcept
        {
            On<EventType>([obj, mem_func](const EventType& e) {
                (obj->*mem_func)(e);
            });
        }


        template <class Et>
        void SetHandler(MessageScheduler<platform, Et>::Handler&& handler)
        {
            auto* scheduler = _get_or_create_scheduler<Et>();
            scheduler->SetHandler(std::move(handler));
        }

        template<typename EventType, typename Class>
        void SetHandler(bool (Class::* mem_func)(const EventType&), Class* obj) {
            SetHandler<EventType>([obj, mem_func](const EventType& e) {
                return (obj->*mem_func)(e);
                });
        }
        

    private:
        template <class Et>
        auto _get_or_create_scheduler() noexcept
        {
            using Scheduler = MessageScheduler<platform, Et>;
            using Converter = EventConverter<platform, Et>;


            const std::type_index type = typeid(Scheduler);

            auto* scheduler = static_cast<Scheduler*>(_get_scheduler(type));
            if (!scheduler)
            {
                scheduler = static_cast<Scheduler*>(_add_scheduler(type, Converter::nativeMsg(), Any{Scheduler{}}));
            }
            return scheduler;
        };
    };



}
