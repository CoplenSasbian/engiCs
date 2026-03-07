#pragma once

#include "platform/platform.h"
namespace nx
{
    constexpr  PlatformMagic PlatformWin32 = make_platform("win32");





    template<>
    struct Platform<PlatformWin32>
    {
        constexpr bool  is_platform_type ()
        {
            return true;
        }
        [[nodiscard]] constexpr static const char* name()
        {
            return "win32";
        }
        [[nodiscard]] static constexpr PlatformMagic magic()
        {
            return PlatformWin32;
        }

        static  constexpr const wchar_t* WndClassNameW()
        {
            return L"nx_win32_wnd_class";
        }
        static  constexpr const char* WndClassNameA()
        {
            return "nx_win32_wnd_class";
        }

        using PlatformCharT = wchar_t;
    };

   using PlatformWin32_t = Platform<PlatformWin32>;


}
#ifdef UNICODE
#define    WND_CLASS_NAME()  nx::Platform<nx::PlatformWin32>::WndClassNameW()
#else
#define WND_CLASS_NAME()  nx::Platform<nx::PlatformWin32>::WndClassNameA()
#endif