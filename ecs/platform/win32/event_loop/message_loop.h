#pragma once
#include <cstdint>
#include <Windows.h>
#include <platform/message_loop/message_loop.h>

namespace nx
{
    class Win32EventLoop: public BaseMessageLoop<PlatformWin32>
    {
    public:

        Win32EventLoop() noexcept;
        bool PoolEvent() noexcept override;
        void PostQuit() noexcept override;
        Error SentMessage(void* rawEvent) noexcept override;
        ~Win32EventLoop() override;
    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg,
            WPARAM wParam, LPARAM lParam) noexcept;
    };



    template<>
    class MessageLoop<PlatformWin32> : public Win32EventLoop{};

}
