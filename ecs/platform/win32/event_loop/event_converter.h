#pragma once
#include <span>
#include "platform/message_loop/event.h"
#include "platform/win32/win32_platform.h"
#include "platform/message_loop/message_loop.h"
namespace nx
{

    template <>
    struct EventConverter<PlatformWin32, EventBase>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static EventBase fromNative(void* rawMsg);
        static void toNative(const EventBase& event, void* rawMsg);
    };

    // Declaration for CloseEvent converter
    template <>
    struct EventConverter<PlatformWin32, CloseEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static CloseEvent fromNative(void* rawMsg);
        static void toNative(const CloseEvent& event, void* rawMsg);
    };

    // Declaration for ResizeEvent converter
    template <>
    struct EventConverter<PlatformWin32, ResizeEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static ResizeEvent fromNative(void* rawMsg);
        static void toNative(const ResizeEvent& event, void* rawMsg);
    };

    // Declaration for MoveEvent converter
    template <>
    struct EventConverter<PlatformWin32, MoveEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static MoveEvent fromNative(void* rawMsg);
        static void toNative(const MoveEvent& event, void* rawMsg);
    };

    // Declaration for FocusEvent converter
    template <>
    struct EventConverter<PlatformWin32, FocusEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static FocusEvent fromNative(void* rawMsg);
        static void toNative(const FocusEvent& event, void* rawMsg);
    };

    // Declaration for LostFocusEvent converter
    template <>
    struct EventConverter<PlatformWin32, LostFocusEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static LostFocusEvent fromNative(void* rawMsg);
        static void toNative(const LostFocusEvent& event, void* rawMsg);
    };

    // Declaration for MouseMoveEvent converter
    template <>
    struct EventConverter<PlatformWin32, MouseMoveEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static MouseMoveEvent fromNative(void* rawMsg);
        static void toNative(const MouseMoveEvent& event, void* rawMsg);
    };

    // Declaration for MousePressEvent converter
    template <>
    struct EventConverter<PlatformWin32, MousePressEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static MousePressEvent fromNative(void* rawMsg);
        static void toNative(const MousePressEvent& event, void* rawMsg);
    };

    // Declaration for MouseReleaseEvent converter
    template <>
    struct EventConverter<PlatformWin32, MouseReleaseEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static MouseReleaseEvent fromNative(void* rawMsg);
        static void toNative(const MouseReleaseEvent& event, void* rawMsg);
    };

    // Declaration for MouseWheelEvent converter
    template <>
    struct EventConverter<PlatformWin32, MouseWheelEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static MouseWheelEvent fromNative(void* rawMsg);
        static void toNative(const MouseWheelEvent& event, void* rawMsg);
    };

    // Declaration for KeyPressEvent converter
    template <>
    struct EventConverter<PlatformWin32, KeyPressEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static KeyPressEvent fromNative(void* rawMsg);
        static void toNative(const KeyPressEvent& event, void* rawMsg);
    };

    // Declaration for KeyReleaseEvent converter
    template <>
    struct EventConverter<PlatformWin32, KeyReleaseEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static KeyReleaseEvent fromNative(void* rawMsg);
        static void toNative(const KeyReleaseEvent& event, void* rawMsg);
    };

    // Declaration for KeyCharEvent converter
    template <>
    struct EventConverter<PlatformWin32, KeyCharEvent>
    {
        static std::span<PlatformMessageType> nativeMsg();
        static KeyCharEvent fromNative(void* rawMsg);
        static void toNative(const KeyCharEvent& event, void* rawMsg);
    };
}
