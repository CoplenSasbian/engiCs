module;
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <locale>

module nx.core.config.console;

void nx::SetupConsole()
{
    std::ios_base::sync_with_stdio(false);

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

#else
    std::locale::global(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());
#endif
}
