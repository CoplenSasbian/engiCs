module;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

module nx.platform.window.win32.eventloop;



nx::Win32EventLoop::Win32EventLoop()
{
   OnIdle = []() {};
}

void nx::Win32EventLoop::Initialize()
{
    
}

void nx::Win32EventLoop::PoolEvents()
{
    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            OnIdle();
        }
       
    }
}

void nx::Win32EventLoop::Shutdown()
{

}