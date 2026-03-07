#pragma once
#include <string>
#include <string_view>
#include "core/error_code.h"
namespace nx
{
    class String{
    public:
        [[nodiscard,deprecated]] static Result<String> from(const std::string& raw,const std::string&  encoding) noexcept;
         [[nodiscard]]static String from( std::string_view raw)noexcept;
        [[nodiscard]]static String from(std::wstring_view raw) noexcept;
        [[nodiscard]]static String from(std::u8string_view raw) noexcept;
        [[nodiscard]]static String from(std::u16string_view raw) noexcept;
        [[nodiscard]]static String from(std::u32string_view raw) noexcept;

        String(std::u16string str) : m_str(std::move(str)) {}
        [[nodiscard]] std::string to_string() const noexcept;
        [[nodiscard]] std::wstring to_wstring() const noexcept;
        [[nodiscard]] std::u8string to_u8string() const noexcept;
        [[nodiscard]] std::u16string to_u16string() const noexcept;
        [[nodiscard]] std::u32string to_u32string() const noexcept;

        String operator+(const String& other) const noexcept;
        String& operator+=(const String& other) noexcept;

    private:
        String()=default;
        std::u16string m_str;

    };





}