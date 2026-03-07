#include "string.h"
#include <string_view>
#include <string>
#include <boost/locale.hpp>




namespace nx {


    nx::Result<nx::String> nx::String::from(const std::string& raw, const std::string&  encoding) noexcept
    {
        try
        {
            auto utf = boost::locale::conv::utf_to_utf<char16_t>(raw);
            return String{utf};
        } catch (const boost::locale::conv::invalid_charset_error& e) {
            return Unexpected(make_error_code(EcsErrc::UnsupportedEncoding));
        } catch (const boost::locale::conv::conversion_error& e) {
            return Unexpected(make_error_code(EcsErrc::ConversionError));
        }
    }

    String String::from(std::string_view raw) noexcept
    {
        return String(
              boost::locale::conv::utf_to_utf<char16_t>(
                     raw.data(),
                     raw.data() + raw.size()));
    }

    nx::String nx::String::from(std::wstring_view raw) noexcept
    {
        if constexpr (sizeof(wchar_t) == 2)
        {
            const char16_t* data = reinterpret_cast<const char16_t*>(raw.data());
            return String( std::u16string(data, raw.length()));
        }else if constexpr (sizeof(wchar_t) == 4)
        {
            return String(
            boost::locale::conv::utf_to_utf<char16_t>(
                   raw.data(),
                   raw.data() + raw.size()
               )
            );
        }
        std::unreachable();

    }

    nx::String nx::String::from(std::u8string_view raw) noexcept
    {

    return String(
       boost::locale::conv::utf_to_utf<char16_t>(
              raw.data(),
              raw.data() + raw.size())
   );

    }

   nx::String nx::String::from(std::u16string_view raw) noexcept
    {
        return String(std::u16string(raw));
    }

nx::String nx::String::from(std::u32string_view raw) noexcept
    {


            return String(
               boost::locale::conv::utf_to_utf<char16_t>(
                      raw.data(),
                      raw.data() + raw.size())
           );

    }


    // --- Conversion Methods (Instance) ---

 std::string nx::String::to_string() const noexcept
    {

            std::string original_back = boost::locale::conv::utf_to_utf<char>(
                  m_str
              );
            return original_back;


    }

    std::wstring nx::String::to_wstring() const noexcept
    {
        if constexpr (sizeof(wchar_t) == 2)
        {
            return std::wstring(reinterpret_cast<const wchar_t*>(m_str.c_str()), m_str.size());
        }else if constexpr (sizeof(wchar_t) == 4)
        {
            return boost::locale::conv::utf_to_utf<wchar_t>(
                m_str.c_str(),
                m_str.c_str() + m_str.size()
            );
        }
    }

    std::u8string nx::String::to_u8string() const noexcept
    {

        return boost::locale::conv::utf_to_utf<char8_t>(
            m_str.c_str(),
            m_str.c_str() + m_str.size()
        );
    }

    std::u16string nx::String::to_u16string() const noexcept
    {

        return m_str;
    }

    std::u32string nx::String::to_u32string() const noexcept
    {
        return boost::locale::conv::utf_to_utf<char32_t>(
          m_str.c_str(),
          m_str.c_str() + m_str.size()
      );
    }

    String String::operator+(const String& other) const noexcept
    {
        return  m_str + other.m_str;
    }

    String& String::operator+=(const String& other) noexcept
    {
        m_str += other.m_str;
        return *this;
    }
} // namespace nx



