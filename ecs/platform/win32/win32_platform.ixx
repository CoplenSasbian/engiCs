//
// Created by futurvo on 2026/3/4.
//

export module nx.platform.win32;
import nx.platform;
namespace nx
{
    export
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
    };

    export using PlatformWin32_t = Platform<PlatformWin32>;


}