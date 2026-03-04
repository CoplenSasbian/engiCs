module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <optional>
#include <system_error>

module nx.platform.win32.event_loop;
import nx.platform.event_loop;


void nx::EventLoop<nx::PlatformWin32>::Pool() noexcept
{
    MSG msg = {};
    ::GetMessage(&msg, nullptr, 0, 0);
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);
}

void nx::EventLoop<nx::PlatformWin32>::PostQuit() noexcept
{
    ::PostQuitMessage(0);
}

void nx::EventLoop<nx::PlatformWin32>::Run() noexcept
{
    MSG msg = {};
    while ( ::GetMessage(&msg, nullptr, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
}

nx::Error nx::EventLoop<nx::PlatformWin32>::SentMessage(void* rawEvent) noexcept
{
    MSG* msg = static_cast<MSG*>(rawEvent);
    if (::PostMessage(msg->hwnd, msg->message, msg->wParam, msg->lParam))
    {
       return std::nullopt;
    }
    std::error_code ec{ static_cast<int>(::GetLastError()), std::system_category() };
    return ec;
}


