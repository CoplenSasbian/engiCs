#pragma once

#include <expected>
#include <optional>
#include <system_error>

namespace nx
{
    enum class EcsErrc
    {
        Success = 0,
        OutOfBounds,
        BufferOverflow,
        
        InvalidEncoding,
        UnsupportedEncoding,
        ConversionError,
        
        LoopStopped,
        InvalidThreadToken,
        InvalidThreadType,
        QueueFull,
        InvalidQueueType,
        NotAvailableWindow,

        Retry,
        UnsuppotShaderType,
        VertexShaderNotFound,

        UnknownError = -1,
    };


    class EcsCategory : public std::error_category
    {
    public:
        [[nodiscard]] const char* name() const noexcept override;;
        [[nodiscard]] std::string message(int code) const override;;
    };

    const std::error_category& gCoreCategory();

    std::error_code make_error_code(EcsErrc e);

    using Error = std::expected<void, std::error_code>;
    extern Error Succeeded;


    template<typename T>
    using Result = std::expected<T,std::error_code>;
    inline
    auto Unexpected(std::error_code ec)
    {
        return std::unexpected(ec);
    };

#if defined(WIN32)
    std::error_code make_system_error();
#endif

}

namespace std {
    template <>
    struct is_error_code_enum<nx::EcsCategory> : true_type {};
}


#define RETURN_ON_ERROR(expr) do { auto _err = (expr); if (!_err) { return nx::Unexpected(_err.error()); } } while(0)
#define ASSIGN_OR_RETURN_ERROR(lhs, expr) do { auto&& _res = (expr); if (!_res.has_value()) { return nx::Unexpected(_res.error()); } (lhs) = std::move(_res.value()); } while (0)



#ifdef  NDEBUG
    #define ECS_DEBUGBREAK() abort();
#else
    #if defined(_MSC_VER)
        #define ECS_DEBUGBREAK() __debugbreak()
    #else
        #define ECS_DEBUGBREAK()  __builtin_trap(); 
    #endif
#endif


#ifdef NDEBUG
    #define ECS_ASSERT(expr) [[assume(expr)]]
#else
    #define ECS_ASSERT(expr)if (!(expr)) { ECS_DEBUGBREAK(); } 
#endif // DEBUG
