#pragma once

#include <string>
 namespace nx {
#if defined(_UNICODE) || defined(UNICODE)
    using tstring = std::wstring;
    using tchar = wchar_t;
    using tchar_ptr = wchar_t*;
    using tchar_c_ptr = const wchar_t*;
    using tchar_view = std::wstring_view;
#else
    using tstring = std::string;
    using tchar = char;
    using tchar_ptr = char*;
    using tchar_c_ptr = const char*;
    using tchar_view = std::string_view;
#endif

}