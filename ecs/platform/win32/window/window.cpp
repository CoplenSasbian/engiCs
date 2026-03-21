#include "window.h"
#include <assert.h>
#include <Windows.h>
#include <system_error>

#if defined(_MSC_VER)
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

nx::Win32Window::~Win32Window()
{
}

bool nx::Win32Window::IsValid() const noexcept
{
    return  m_hwnd && IsWindow(static_cast<HWND>(m_hwnd));
}

nx::Error nx::Win32Window::Create(String title, const Rect& rect) noexcept
{


    m_hwnd =CreateWindowEx(NULL,WND_CLASS_NAME(),title.to_wstring().c_str(),
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,500,500,
        500,500,NULL,NULL,::GetModuleHandle(0),this);

    if (!m_hwnd)
    {
		return make_system_error();
    }

    return Succeeded;
}

void nx::Win32Window::Activate() noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    ::SetFocus(hwnd);
    ::UpdateWindow(hwnd);
}

void nx::Win32Window::Close() noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    CloseWindow(hwnd);
}

void nx::Win32Window::Destroy() noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    ::DestroyWindow(hwnd);
}

nx::Result<nx::Rect> nx::Win32Window::GetClientArea() const noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    if ( Rect rect;::GetClientRect(hwnd,reinterpret_cast<RECT*>(&rect)))
    {
        return  rect;
    }
    return Unexpected(make_system_error());
}

nx::Result<nx::Rect> nx::Win32Window::GetWindowArea() const noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);

    if ( Rect rect;::GetWindowRect(hwnd,reinterpret_cast<RECT*>(&rect)))
    {
        return  rect;
    }
    return Unexpected(make_system_error());
}

bool nx::Win32Window::IsVisible() const noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    LONG style = ::GetWindowLong(hwnd,GWL_STYLE);
    return style & WS_VISIBLE;
}

nx::Error nx::Win32Window::Maximize() noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    if (ShowWindow(hwnd,SW_MAXIMIZE))
    {
        return Succeeded;
    }
    return  make_system_error();
}

nx::Error nx::Win32Window::Minimize() noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    if (ShowWindow(hwnd,SW_MINIMIZE))
    {
        return Succeeded;
    }
    return  make_system_error();
}

nx::NativeWindowHandle nx::Win32Window::NativeHandle() const noexcept
{
    return  m_hwnd;
}

nx::Error nx::Win32Window::Restore() noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    if (ShowWindow(hwnd,SW_RESTORE))
    {
        return Succeeded;
    }
    return make_system_error();
}

nx::Error nx::Win32Window::SetVisible(bool show) noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    if (ShowWindow(hwnd,show?SW_SHOW:SW_HIDE))
    {
        return Succeeded;
    }
    return  make_system_error();
}

nx::Error nx::Win32Window::SetWindowArea(const Rect& rect) noexcept
{
    HWND hwnd = static_cast<HWND>(m_hwnd);
    assert(hwnd);
    if (::SetWindowPos(hwnd,NULL,rect.x,rect.y,rect.width,rect.height,SWP_NOZORDER))
    {
        return Succeeded;
    }
    return make_system_error();
}


static
UINT getWinIcon(nx::IMessageBox::Icon icon) {
    using enum nx::IMessageBox::Icon;
    switch (icon) {
    case Info:     return MB_ICONINFORMATION;
    case Warning:  return MB_ICONWARNING;
    case Error:    return MB_ICONERROR;
    case Question: return MB_ICONQUESTION;
    default:             return MB_OK;
    }
}

static
UINT getWinButtons(nx::IMessageBox::Button button) {
    using enum nx::IMessageBox::Button;
    switch (button) {
    case OK:           return MB_OK;
    case OKCancel:     return MB_OKCANCEL;
    case YesNo:        return MB_YESNO;
    case YesNoCancel:  return MB_YESNOCANCEL;
    default:                   return MB_OK;
    }
}


static
nx::IMessageBox::Result fromWinResult(int winResult) {
    using enum  nx::IMessageBox::Result;
    switch (winResult) {
    case IDOK:     return OK;
    case IDCANCEL: return Cancel;
    case IDYES:    return Yes;
    case IDNO:     return No;
    default:       return Cancel; 
    }
}


nx::IMessageBox::Result nx::Wind32MessageBox::show(const String& title, const String& message, Icon icon,
    Button buttons)
{
    UINT style = 0;

    style = getWinIcon(icon) | getWinButtons(buttons);
    auto ret = ::MessageBoxW(NULL,message.to_wstring().c_str(),title.to_wstring().c_str(),style);
   return  fromWinResult(ret);
}


