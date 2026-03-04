export module nx.platform.win32.event_looop.event;
import nx.platform.win32;
export import nx.platform.event_loop.event;
import nx.platform.event_loop;
export namespace nx
{


    template <>
    struct EventConverter<PlatformWin32, EventBase>
    {
        static EventBase fromNative(void* rawMsg);
        static void toNative(const EventBase& event, void* rawMsg);
    };

    // Declaration for CloseEvent converter
    template <>
    struct EventConverter<PlatformWin32, CloseEvent>
    {
        static CloseEvent fromNative(void* rawMsg);
        static void toNative(const CloseEvent& event, void* rawMsg);
    };

    // Declaration for ResizeEvent converter
    template <>
    struct EventConverter<PlatformWin32, ResizeEvent>
    {
        static ResizeEvent fromNative(void* rawMsg);
        static void toNative(const ResizeEvent& event, void* rawMsg);
    };

    // Declaration for MoveEvent converter
    template <>
    struct EventConverter<PlatformWin32, MoveEvent>
    {
        static MoveEvent fromNative(void* rawMsg);
        static void toNative(const MoveEvent& event, void* rawMsg);
    };

    // Declaration for FocusEvent converter
    template <>
    struct EventConverter<PlatformWin32, FocusEvent>
    {
        static FocusEvent fromNative(void* rawMsg);
        static void toNative(const FocusEvent& event, void* rawMsg);
    };

    // Declaration for LostFocusEvent converter
    template <>
    struct EventConverter<PlatformWin32, LostFocusEvent>
    {
        static LostFocusEvent fromNative(void* rawMsg);
        static void toNative(const LostFocusEvent& event, void* rawMsg);
    };

    // Declaration for MouseMoveEvent converter
    template <>
    struct EventConverter<PlatformWin32, MouseMoveEvent>
    {
        static MouseMoveEvent fromNative(void* rawMsg);
        static void toNative(const MouseMoveEvent& event, void* rawMsg);
    };

    // Declaration for MousePressEvent converter
    template <>
    struct EventConverter<PlatformWin32, MousePressEvent>
    {
        static MousePressEvent fromNative(void* rawMsg);
        static void toNative(const MousePressEvent& event, void* rawMsg);
    };

    // Declaration for MouseReleaseEvent converter
    template <>
    struct EventConverter<PlatformWin32, MouseReleaseEvent>
    {
        static MouseReleaseEvent fromNative(void* rawMsg);
        static void toNative(const MouseReleaseEvent& event, void* rawMsg);
    };

    // Declaration for MouseWheelEvent converter
    template <>
    struct EventConverter<PlatformWin32, MouseWheelEvent>
    {
        static MouseWheelEvent fromNative(void* rawMsg);
        static void toNative(const MouseWheelEvent& event, void* rawMsg);
    };

    // Declaration for KeyPressEvent converter
    template <>
    struct EventConverter<PlatformWin32, KeyPressEvent>
    {
        static KeyPressEvent fromNative(void* rawMsg);
        static void toNative(const KeyPressEvent& event, void* rawMsg);
    };

    // Declaration for KeyReleaseEvent converter
    template <>
    struct EventConverter<PlatformWin32, KeyReleaseEvent>
    {
        static KeyReleaseEvent fromNative(void* rawMsg);
        static void toNative(const KeyReleaseEvent& event, void* rawMsg);
    };

    // Declaration for KeyCharEvent converter
    template <>
    struct EventConverter<PlatformWin32, KeyCharEvent>
    {
        static KeyCharEvent fromNative(void* rawMsg);
        static void toNative(const KeyCharEvent& event, void* rawMsg);
    };
}
