#include "event_converter.h"
#include <windows.h>
#include <windowsx.h>

namespace nx
{
    // Helper function to convert Win32 WPARAM/LPARAM to Nx MouseButton
    static MouseButton GetMouseButtonFromWParam(WPARAM wParam)
    {
        if (wParam & MK_LBUTTON) return MouseButton::Left;
        if (wParam & MK_RBUTTON) return MouseButton::Right;
        if (wParam & MK_MBUTTON) return MouseButton::Middle;
        if (wParam & MK_XBUTTON1) return MouseButton::X1;
        if (wParam & MK_XBUTTON2) return MouseButton::X2;
        return MouseButton::Left; // Fallback
    }

    // Helper function to convert Win32 WPARAM to Nx Key
    static Key GetKeyFromWParam(WPARAM wParam)
    {
        switch (wParam)
        {
        case VK_BACK: return Key::Backspace;
        case VK_TAB: return Key::Tab;
        case VK_RETURN: return Key::Enter;
        case VK_ESCAPE: return Key::Escape;
        case VK_SPACE: return Key::Space;
        case VK_LEFT: return Key::Left;
        case VK_UP: return Key::Up;
        case VK_RIGHT: return Key::Right;
        case VK_DOWN: return Key::Down;
        case VK_PRIOR: return Key::PageUp;
        case VK_NEXT: return Key::PageDown;
        case VK_END: return Key::End;
        case VK_HOME: return Key::Home;
        case VK_INSERT: return Key::Insert;
        case VK_DELETE: return Key::Delete;
        case VK_F1: return Key::F1;
        case VK_F2: return Key::F2;
        case VK_F3: return Key::F3;
        case VK_F4: return Key::F4;
        case VK_F5: return Key::F5;
        case VK_F6: return Key::F6;
        case VK_F7: return Key::F7;
        case VK_F8: return Key::F8;
        case VK_F9: return Key::F9;
        case VK_F10: return Key::F10;
        case VK_F11: return Key::F11;
        case VK_F12: return Key::F12;
        case VK_NUMPAD0: return Key::Numpad0;
        case VK_NUMPAD1: return Key::Numpad1;
        case VK_NUMPAD2: return Key::Numpad2;
        case VK_NUMPAD3: return Key::Numpad3;
        case VK_NUMPAD4: return Key::Numpad4;
        case VK_NUMPAD5: return Key::Numpad5;
        case VK_NUMPAD6: return Key::Numpad6;
        case VK_NUMPAD7: return Key::Numpad7;
        case VK_NUMPAD8: return Key::Numpad8;
        case VK_NUMPAD9: return Key::Numpad9;
        case VK_ADD: return Key::NumpadAdd;
        case VK_SUBTRACT: return Key::NumpadSubtract;
        case VK_MULTIPLY: return Key::NumpadMultiply;
        case VK_DIVIDE: return Key::NumpadDivide;
        case VK_DECIMAL: return Key::NumpadDecimal;
        case VK_SHIFT: return Key::LShift;
        case VK_CONTROL: return Key::LControl;
        case VK_MENU: return Key::LAlt;
        default: return static_cast<Key>(wParam);
        }
    }


    // --- Implementation for EventBase converter ---
    EventBase EventConverter<PlatformWin32, EventBase>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        auto window = reinterpret_cast<IWindow*>(::GetWindowLongPtrW(msg->hwnd,GWLP_USERDATA));
        return EventBase{.window = window};
    }

    void EventConverter<PlatformWin32, EventBase>::toNative(const EventBase& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        msg->hwnd = static_cast<HWND>(event.window->NativeHandle());
        msg->message = WM_NULL;
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, EventBase>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {WM_NULL};
        return {arr};
    }

    // --- Implementation for CloseEvent converter ---
    CloseEvent EventConverter<PlatformWin32, CloseEvent>::fromNative(void* rawMsg)
    {
        return CloseEvent{ EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg)};
    }


    void EventConverter<PlatformWin32, CloseEvent>::toNative(const CloseEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_CLOSE;
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, CloseEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {WM_CLOSE};
        return arr;
    }

    // --- Implementation for ResizeEvent converter ---
    ResizeEvent EventConverter<PlatformWin32, ResizeEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        return ResizeEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            LOWORD(msg->lParam),
            HIWORD(msg->lParam)
        };
    }

    void EventConverter<PlatformWin32, ResizeEvent>::toNative(const ResizeEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_SIZE;
        msg->lParam = MAKELONG(event.width, event.height);
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, ResizeEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {WM_SIZE};
        return arr;
    }

    // --- Implementation for MoveEvent converter ---
    MoveEvent EventConverter<PlatformWin32, MoveEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        return MoveEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            static_cast<int>(GET_X_LPARAM(msg->lParam)),
            static_cast<int>(GET_Y_LPARAM(msg->lParam))
        };
    }

    void EventConverter<PlatformWin32, MoveEvent>::toNative(const MoveEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_MOVE;
        msg->lParam = MAKELONG(event.x, event.y);
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, MoveEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {WM_MOVE};
        return arr;
    }
    // --- Implementation for FocusEvent converter ---
    FocusEvent EventConverter<PlatformWin32, FocusEvent>::fromNative(void* rawMsg)
    {
        return FocusEvent{EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg)};
    }

    void EventConverter<PlatformWin32, FocusEvent>::toNative(const FocusEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_SETFOCUS;
        msg->lParam = 0;
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, FocusEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {WM_SETFOCUS};
        return arr;
    }
    // --- Implementation for LostFocusEvent converter ---
    LostFocusEvent EventConverter<PlatformWin32, LostFocusEvent>::fromNative(void* rawMsg)
    {
        return LostFocusEvent{EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg)};
    }

    void EventConverter<PlatformWin32, LostFocusEvent>::toNative(const LostFocusEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_KILLFOCUS;
        msg->lParam = 0;
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, LostFocusEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {WM_KILLFOCUS};
        return arr;
    }

    // --- Implementation for MouseMoveEvent converter ---
    MouseMoveEvent EventConverter<PlatformWin32, MouseMoveEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        return MouseMoveEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            static_cast<float>(GET_X_LPARAM(msg->lParam)),
            static_cast<float>(GET_Y_LPARAM(msg->lParam))
        };
    }

    void EventConverter<PlatformWin32, MouseMoveEvent>::toNative(const MouseMoveEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_MOUSEMOVE;
        msg->lParam = MAKELONG(static_cast<LONG>(event.x), static_cast<LONG>(event.y));
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, MouseMoveEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {WM_MOUSEMOVE};
        return arr;
    }
    // --- Implementation for MousePressEvent converter ---
    MousePressEvent EventConverter<PlatformWin32, MousePressEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        MouseButton button = MouseButton::Left; // Default
        switch (msg->message)
        {
        case WM_LBUTTONDOWN: button = MouseButton::Left;
            break;
        case WM_RBUTTONDOWN: button = MouseButton::Right;
            break;
        case WM_MBUTTONDOWN: button = MouseButton::Middle;
            break;
        case WM_XBUTTONDOWN:
            button = (HIWORD(msg->wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;
            break;
        }

        return MousePressEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            button,
            static_cast<float>(GET_X_LPARAM(msg->lParam)),
            static_cast<float>(GET_Y_LPARAM(msg->lParam))
        };
    }

    void EventConverter<PlatformWin32, MousePressEvent>::toNative(const MousePressEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        switch (event.button)
        {
        case MouseButton::Left: msg->message = WM_LBUTTONDOWN;
            break;
        case MouseButton::Right: msg->message = WM_RBUTTONDOWN;
            break;
        case MouseButton::Middle: msg->message = WM_MBUTTONDOWN;
            break;
        case MouseButton::X1: msg->message = WM_XBUTTONDOWN;
            msg->wParam = MAKEWPARAM(0, XBUTTON1);
            break;
        case MouseButton::X2: msg->message = WM_XBUTTONDOWN;
            msg->wParam = MAKEWPARAM(0, XBUTTON2);
            break;
        }
        msg->lParam = MAKELONG(static_cast<LONG>(event.x), static_cast<LONG>(event.y));
    }
    std::span<PlatformMessageType> EventConverter<PlatformWin32, MousePressEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {
            WM_LBUTTONDOWN,
            WM_RBUTTONDOWN,
            WM_MBUTTONDOWN,
            WM_XBUTTONDOWN
        };
        return arr;
    }

    // --- Implementation for MouseReleaseEvent converter ---
    MouseReleaseEvent EventConverter<PlatformWin32, MouseReleaseEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        MouseButton button = MouseButton::Left; // Default
        switch (msg->message)
        {
        case WM_LBUTTONUP: button = MouseButton::Left;
            break;
        case WM_RBUTTONUP: button = MouseButton::Right;
            break;
        case WM_MBUTTONUP: button = MouseButton::Middle;
            break;
        case WM_XBUTTONUP:
            button = (HIWORD(msg->wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;
            break;
        }

        return MouseReleaseEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            button,
            static_cast<float>(GET_X_LPARAM(msg->lParam)),
            static_cast<float>(GET_Y_LPARAM(msg->lParam))
        };
    }

    void EventConverter<PlatformWin32, MouseReleaseEvent>::toNative(const MouseReleaseEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        switch (event.button)
        {
        case MouseButton::Left: msg->message = WM_LBUTTONUP;
            break;
        case MouseButton::Right: msg->message = WM_RBUTTONUP;
            break;
        case MouseButton::Middle: msg->message = WM_MBUTTONUP;
            break;
        case MouseButton::X1: msg->message = WM_XBUTTONUP;
            msg->wParam = MAKEWPARAM(0, XBUTTON1);
            break;
        case MouseButton::X2: msg->message = WM_XBUTTONUP;
            msg->wParam = MAKEWPARAM(0, XBUTTON2);
            break;
        }
        msg->lParam = MAKELONG(static_cast<LONG>(event.x), static_cast<LONG>(event.y));
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, MouseReleaseEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {
            WM_LBUTTONUP,
            WM_RBUTTONUP,
            WM_MBUTTONUP,
            WM_XBUTTONUP
        };
        return arr;
    }
    // --- Implementation for MouseWheelEvent converter ---
    MouseWheelEvent EventConverter<PlatformWin32, MouseWheelEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        short wheelDelta = GET_WHEEL_DELTA_WPARAM(msg->wParam);
        float delta_y = static_cast<float>(wheelDelta) / WHEEL_DELTA;
        // Horizontal wheel is not standard in all messages, this handles WM_MOUSEHWHEEL
        float delta_x = 0.0f;
        if (msg->message == WM_MOUSEHWHEEL)
        {
            delta_x = -delta_y; // Typically, horizontal scroll value is inverted
            delta_y = 0.0f;
        }
        return MouseWheelEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            delta_x,
            delta_y,
            static_cast<float>(GET_X_LPARAM(msg->lParam)),
            static_cast<float>(GET_Y_LPARAM(msg->lParam))
        };
    }

    void EventConverter<PlatformWin32, MouseWheelEvent>::toNative(const MouseWheelEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        if (event.delta_x != 0.0f)
        {
            msg->message = WM_MOUSEHWHEEL;
            msg->wParam = MAKEWPARAM(0, static_cast<SHORT>(-event.delta_x * WHEEL_DELTA)); // Invert for horizontal
        }
        else
        {
            msg->message = WM_MOUSEWHEEL;
            msg->wParam = MAKEWPARAM(0, static_cast<SHORT>(event.delta_y * WHEEL_DELTA));
        }
        msg->lParam = MAKELONG(static_cast<LONG>(event.x), static_cast<LONG>(event.y));
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, MouseWheelEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {
            WM_MOUSEHWHEEL
        };
        return arr;
    }
    // --- Implementation for KeyPressEvent converter ---
    KeyPressEvent EventConverter<PlatformWin32, KeyPressEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        bool isRepeat = (msg->lParam & (1 << 30)) != 0;
        return KeyPressEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            GetKeyFromWParam(msg->wParam),
            isRepeat
        };
    }

    void EventConverter<PlatformWin32, KeyPressEvent>::toNative(const KeyPressEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_KEYDOWN;
        msg->wParam = static_cast<WPARAM>(event.key_code);
        // Setting lParam for repeat count and other flags is complex, simplified here
        if (event.is_repeat)
        {
            msg->lParam |= (1 << 30);
        }
    }
    std::span<PlatformMessageType> EventConverter<PlatformWin32, KeyPressEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {
            WM_KEYDOWN
        };
        return arr;
    }
    // --- Implementation for KeyReleaseEvent converter ---
    KeyReleaseEvent EventConverter<PlatformWin32, KeyReleaseEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        return KeyReleaseEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            GetKeyFromWParam(msg->wParam)
        };
    }

    void EventConverter<PlatformWin32, KeyReleaseEvent>::toNative(const KeyReleaseEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_KEYUP;
        msg->wParam = static_cast<WPARAM>(event.key_code);
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, KeyReleaseEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {
            WM_KEYUP
        };
        return arr;
    }
    // --- Implementation for KeyCharEvent converter ---
    KeyCharEvent EventConverter<PlatformWin32, KeyCharEvent>::fromNative(void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        return KeyCharEvent{
            EventConverter<PlatformWin32, EventBase>::fromNative(rawMsg),
            static_cast<std::uint32_t>(msg->wParam)
        };
    }

    void EventConverter<PlatformWin32, KeyCharEvent>::toNative(const KeyCharEvent& event, void* rawMsg)
    {
        MSG* msg = static_cast<MSG*>(rawMsg);
        EventConverter<PlatformWin32, EventBase>::toNative(event, rawMsg);
        msg->message = WM_CHAR;
        msg->wParam = static_cast<WPARAM>(event.character);
    }

    std::span<PlatformMessageType> EventConverter<PlatformWin32, KeyCharEvent>::nativeMsg()
    {
        static  PlatformMessageType arr[] = {
            WM_CHAR
        };
        return arr;
    }
} // namespace nx
