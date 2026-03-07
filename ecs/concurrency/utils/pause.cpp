#include "pause.h"
#ifdef _WIN32
#include <intrin.h>
#endif

#include <thread>  // For fallback


void nx::__Pause() noexcept
{
#if defined(_WIN32)
    // Windows platforms
#if defined(_M_X64) || defined(_M_IX86)
    _mm_pause();           // x86/x64
#elif defined(_M_ARM) || defined(_M_ARM64)
    __yield();              // ARM
#else
    std::this_thread::yield();
#endif

#elif defined(__x86_64__) || defined(__i386__) || defined(__amd64__)
    // Linux/Unix x86/x64
#if defined(__GNUC__) || defined(__clang__)
    __builtin_ia32_pause();
#else
    __asm__ volatile("pause" ::: "memory");
#endif

#elif defined(__aarch64__) || defined(__arm__)
    // Linux/Unix ARM
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("yield" ::: "memory");
#else
    __asm__ volatile("yield" ::: "memory");
#endif

#else
    // Unknown platform fallback
    std::this_thread::yield();
#endif
}