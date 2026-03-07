#include "message_loop.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <system_error>





static nx::Error registerDefWindowClass(WNDPROC  proc)
{
#ifdef UNICODE
    #define    WND_CLASS_NAME()  nx::Platform<nx::PlatformWin32>::WndClassNameW()
#else
    #define WND_CLASS_NAME()  nx::Platform<nx::PlatformWin32>::WndClassNameA()
#endif



    static bool registered = false;
    if (!registered)
    {
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = proc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = ::GetModuleHandle(0);
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = WND_CLASS_NAME();
        wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
        if (!RegisterClassEx(&wcex))
        {
            return std::error_code(::GetLastError(), std::system_category());
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
    std::error_code ec{ static_cast<int>(::GetLastError()), std::system_category() };
    return ec;
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


