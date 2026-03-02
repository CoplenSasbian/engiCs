module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <cstdint>
#include <memory>
#include <optional>
module nx.platform.window.win32;
import nx.core.memory;
import nx.platform.error_code;

struct WndClassHelper {
    explicit  WndClassHelper( WNDPROC wndProc)
   {
      WNDCLASSEX wndcex = {};
      wndcex.cbSize = sizeof(WNDCLASSEX);
      wndcex.style = CS_HREDRAW | CS_VREDRAW;
      wndcex.lpfnWndProc = wndProc;
      wndcex.hInstance = GetModuleHandle(nullptr);
      wndcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
      wndcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
      wndcex.lpszClassName = TEXT("Win32Window");

      if (!RegisterClassEx(&wndcex)) {
        erroc = nx::make_system_error();
      }
   }

    nx::Error erroc;
};



nx::Win32Window::Win32Window() : m_window(nullptr) {
    m_threadId = GetCurrentThreadId();
}

void* nx::Win32Window::NativeHandle()
{
    return reinterpret_cast<void*>(m_window);
}




nx::Error nx::Win32Window::Initialize()
{
    static WndClassHelper wndClassHelper(WndProc);

    if (wndClassHelper.erroc) return  wndClassHelper.erroc;

    m_window = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        TEXT("Win32Window"),
        TEXT(""),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this
    );

    if (!m_window)
    {
        return make_system_error();
    }
    return NoError;
}

void nx::Win32Window::Shutdown()
{
    DestroyWindow(m_window);
    m_window = nullptr;
}

void nx::Win32Window::Resize(int width, int height)
{
    RECT rect;
    GetWindowRect(m_window, &rect);
    MoveWindow(m_window, rect.left, rect.top, width, height, TRUE);
}

void nx::Win32Window::Move(int x, int y)
{
    RECT rect;
    GetWindowRect(m_window, &rect);
    MoveWindow(m_window, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

void nx::Win32Window::SetTitle(tchar_view title)
{
    SetWindowText(m_window, title.data());
}

void nx::Win32Window::SetVisible(bool visible)
{
    ShowWindow(m_window, visible ? SW_SHOW : SW_HIDE);
}

void nx::Win32Window::SetCursorVisible(bool visible)
{
    //SetCursor(visible ? LoadCursor(nullptr, IDC_ARROW) : nullptr);
    ShowCursor(visible);
}

void nx::Win32Window::SetCursorLocked(bool locked)
{
    if (locked) {
        RECT rect;
        GetWindowRect(m_window, &rect);
        ClipCursor(&rect);
    }
    else {
        ClipCursor(nullptr);
    }
                 
}

void nx::Win32Window::SetCursorPosition(double x, double y)
{
    SetCursorPos(static_cast<int>(x), static_cast<int>(y));
}



LRESULT nx::Win32Window::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static char buffer[123];
    bool needHandle = true;
#define Block(ET,HAN,BRO,...)\
    {\
      ET e{this,__VA_ARGS__};\
      needHandle = HAN (e);\
    if (e.NeedBroadcast()){\
      BRO.Emit(e);\
      }\
    }



    switch (uMsg) {
    case WM_CREATE:
        Block(ECreate,OnCreate,OnCreated);
        break;
    case WM_CLOSE:
        Block(EClose,OnClose,OnClosed);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        Block(EResize,OnResize,OnResized,width,height);
        break;
    }
    case WM_MOVE: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        Block(EMove,OnMove,OnMoved,x,y);
        break;
    }
    case WM_SETFOCUS:
        Block(EFocus,OnFocus,OnFocused);
        break;
    case WM_KILLFOCUS:
        Block(EBlur,OnBlur,OnBlured);
        break;
    case WM_MOUSEMOVE: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        Block(nx::EMouseMove,OnMouseMove,OnMouseMoved,x,y);
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (uMsg == WM_LBUTTONDOWN) {
            Block(nx::EMouseButton,OnMouseButton,OnMouseButtoned,nx::EMouseButton::Button::Left,true,x,y);
        }
        else if (uMsg == WM_RBUTTONDOWN) {
            Block(nx::EMouseButton,OnMouseButton,OnMouseButtoned,nx::EMouseButton::Button::Right,true,x,y);
        }
        else if (uMsg == WM_MBUTTONDOWN) {
            Block(nx::EMouseButton,OnMouseButton,OnMouseButtoned,nx::EMouseButton::Button::Middle,true,x,y);
        }
        break;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (uMsg == WM_LBUTTONUP) {
            Block(nx::EMouseButton,OnMouseButton,OnMouseButtoned,nx::EMouseButton::Button::Left,false,x,y);
        }
        else if (uMsg == WM_RBUTTONUP) {
            Block(nx::EMouseButton,OnMouseButton,OnMouseButtoned,nx::EMouseButton::Button::Right,false,x,y);
        }
        else if (uMsg == WM_MBUTTONUP) {
            Block(nx::EMouseButton,OnMouseButton,OnMouseButtoned,nx::EMouseButton::Button::Middle,false,x,y);
        }
        break;
    }
    case WM_MOUSEWHEEL: {
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        double yOff = delta / static_cast<double>(WHEEL_DELTA);
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        Block(nx::EMouseWheel,OnMouseWheel,OnMouseWheeled,yOff,x,y);
        break;
    }
    case WM_KEYDOWN:
    case WM_KEYUP: {
        int key = static_cast<int>(wParam);
        bool pressed = (uMsg == WM_KEYDOWN);
        int scancode = (lParam >> 16) & 0xFF;
        int modifiers = 0;
        Block(nx::EKey,OnKey,OnKeyed,key,pressed,scancode,modifiers);
        break;
    }
    case WM_CHAR: {
        uint32_t code = static_cast<uint32_t>(wParam);
        Block(nx::EChar,OnChar,OnCharred,code);
        break;
    }
    }


    if (needHandle)[[likely]] {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return S_OK;

#undef Block
}

LRESULT __stdcall nx::Win32Window::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_CREATE)[[unlikely]]{
    
      CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
      Win32Window* window = static_cast<Win32Window*>(create->lpCreateParams);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
      window->m_window = hwnd;
      return window->HandleMessage(hwnd, uMsg, wParam, lParam);
  }
  else if(auto window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))) [[likely]]{
      return window->HandleMessage(hwnd, uMsg, wParam, lParam);
  }


    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


