module;

export module nx.platform.win32.event_loop;
export import nx.platform.event_loop;
import nx.platform.win32;
import nx.core.memory;


namespace nx
{



    export
    template <>
    class EventLoop<PlatformWin32> : public IEventloop
    {
    public:
        [[nodiscard]] constexpr PlatformMagic platform_type() const override
        {
            return PlatformWin32_t::magic();
        }

        constexpr  size_t native_msg_size() noexcept override
        {
            return 64;
        };


        EventLoop() noexcept = default;
        void Pool() noexcept override;
        void PostQuit() noexcept override;
        void Run() noexcept override;
        Error SentMessage(void* rawEvent) noexcept override;
        ~EventLoop() override = default;
    };

    export
    using Win32EventLoop = EventLoop<PlatformWin32>;
}
