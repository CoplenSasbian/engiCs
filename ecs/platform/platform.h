#pragma once

#include <bit>
#include <cstdint>
#include <string_view>

namespace nx
{
    using PlatformMagic = int64_t;








    consteval PlatformMagic make_platform(std::string_view name)
    {
        if (name.size() > sizeof(PlatformMagic))
        {
            throw "platform name to long";
        }

        alignas(PlatformMagic) unsigned char buffer[sizeof(PlatformMagic)] = {0};

        for (std::size_t i = 0; i < name.size(); ++i)
        {
            buffer[i] = static_cast<unsigned char>(name[i]);
        }

        return std::bit_cast<PlatformMagic>(buffer);
    }

    constexpr PlatformMagic UnknownPlatform = make_platform("Unknown");

    template <PlatformMagic T>
    struct Platform;


    template <>
    struct Platform<UnknownPlatform>
    {
        static constexpr bool is_platform_type() { return false; };

        [[nodiscard]] constexpr static const char* name()
        {
            return "Unknown";
        }

        [[nodiscard]] static constexpr PlatformMagic magic()
        {
            return UnknownPlatform;
        }

        using PlatformCharT = char;
    };

    template <PlatformMagic T>
    bool is_platform_type()
    {
        return Platform<T>::is_platform_type;
    }

    template <PlatformMagic T>
    const char* platform_name()
    {
        return Platform<T>::name();
    }
}
