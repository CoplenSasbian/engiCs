#pragma once
#include "platform/win32/win32_platform.h"
#include "platform/window/window.h"
namespace nx
{
    class Win32Window:public IWindow
    {
    public:
        virtual ~Win32Window();
        [[nodiscard]] bool IsValid() const noexcept override;
        Error Create(String title, const Rect& rect) noexcept override;
        void Activate() noexcept override;
        void Close() noexcept override;
        void Destroy() noexcept override;
        [[nodiscard]] Result<Rect> GetClientArea() const noexcept override;
        [[nodiscard]] Result<Rect> GetWindowArea() const noexcept override;
        [[nodiscard]] bool IsVisible() const noexcept override;
        Error Maximize() noexcept override;
        Error Minimize() noexcept override;
        [[nodiscard]] void* NativeHandle() const noexcept override;
        Error Restore() noexcept override;
        Error SetVisible(bool show) noexcept override;
        Error SetWindowArea(const Rect& rect) noexcept override;
    private:
        void* m_hwnd = nullptr;
    };


    class Wind32MessageBox:public IMessageBox
    {
    public:
        ~Wind32MessageBox() override = default;
        Result show(const String& title, const String& message, Icon icon, Button buttons) override;
    };


    template<>
    class Window<PlatformWin32> :public Win32Window{};


 template<>
    class EMessageBox<PlatformWin32> :public Wind32MessageBox{};
}