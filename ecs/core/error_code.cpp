#include "error_code.h"
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "error_code.h"
#include "error_code.h"
#include "error_code.h"
#endif

const char* nx::EcsCategory::name() const noexcept
{
    return  "co Core";
}

std::string nx::EcsCategory::message(int code) const
{
    switch (static_cast<EcsErrc>(code))
    {
    case EcsErrc::Success:
        return "Success";
    case EcsErrc::OutOfBounds:
        return "Out of bounds";
    case EcsErrc::InvalidEncoding:
        return "Invalid encoding";
    case EcsErrc::UnsupportedEncoding:
        return "The specified encoding is not supported";
    case EcsErrc::ConversionError:
        return "Character encoding/decoding failed";
    case  EcsErrc::LoopStopped:
        return "Loop is stopped";
    case EcsErrc::InvalidThreadToken:
        return "Invalid thread token";
    case EcsErrc::InvalidThreadType:
        return "Invalid thread type";
    case EcsErrc::QueueFull:
        return "Task queue Full";
    case EcsErrc::NotAvailableWindow:
        return "Not available window";
    case EcsErrc::Retry:
		return "Retry";
    case EcsErrc::UnknownError:
    default:
        return "Unknown";
    }
}

const std::error_category& nx::gCoreCategory()
{
    static EcsCategory c;
    return c;
}

std::error_code nx::make_error_code(EcsErrc e)
{
    return std::error_code{static_cast<int>(e), gCoreCategory()};
}



#if defined(WIN32)
std::error_code nx::make_system_error()
{
    return std::error_code(GetLastError(), std::system_category());
}
#endif

