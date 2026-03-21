#include "message_loop.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


#ifdef UNICODE
    #define    WND_CLASS_NAME()  nx::Platform<nx::PlatformWin32>::WndClassNameW()
#else
    #define WND_CLASS_NAME()  nx::Platform<nx::PlatformWin32>::WndClassNameA()
#endif




static nx::Error registerDefWindowClass(WNDPROC  proc)
{




    static bool registered = false;
    if (!registered)
    {
        WNDCLASSEX wndclassexw;
        wndclassexw.cbSize = sizeof(WNDCLASSEX);
        wndclassexw.style = CS_HREDRAW | CS_VREDRAW;
        wndclassexw.lpfnWndProc = proc;
        wndclassexw.cbClsExtra = 0;
        wndclassexw.cbWndExtra = 0;
        wndclassexw.hInstance = ::GetModuleHandle(0);
        wndclassexw.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wndclassexw.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wndclassexw.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wndclassexw.lpszMenuName = nullptr;
        wndclassexw.lpszClassName = WND_CLASS_NAME();
        wndclassexw.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);
        if (!RegisterClassEx(&wndclassexw))
        {
            return nx::make_system_error();
        }
        registered = true;
    }
    return nx::Succeeded;
}



thread_local  nx::Win32EventLoop* tCurrentEventLoop = nullptr;

nx::Win32EventLoop::Win32EventLoop() noexcept
: nx::BaseMessageLoop<PlatformWin32>{}
{
    if (registerDefWindowClass(WindowProc))
    {
        abort();
    }
    tCurrentEventLoop = this;
}

bool nx::Win32EventLoop::PoolEvent() noexcept
{

    MSG msg = {};
    if (::PeekMessageW(&msg, NULL, NULL, NULL, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
    return  msg.message != WM_QUIT;
}

void nx::Win32EventLoop::PostQuit() noexcept
{
    ::PostQuitMessage(0);
}



nx::Error nx::Win32EventLoop::SentMessage(void* rawEvent) noexcept
{
    MSG* msg = static_cast<MSG*>(rawEvent);
    if (::PostMessage(msg->hwnd, msg->message, msg->wParam, msg->lParam))
    {
       return Succeeded;
    }
    return make_system_error();
}

nx::Win32EventLoop::~Win32EventLoop()
{
    tCurrentEventLoop = nullptr;
}


LRESULT CALLBACK nx::Win32EventLoop::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{

    if (uMsg == WM_CREATE) [[unlikely]]
    {
        auto pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcs->lpCreateParams));
    }

    MSG msg = {hwnd, uMsg, wParam, lParam };
    if (tCurrentEventLoop->dispatch(&msg, uMsg))
    {
        return ::DefWindowProc(static_cast<HWND>(hwnd), uMsg, wParam, lParam);
    }
    return S_OK;
}


