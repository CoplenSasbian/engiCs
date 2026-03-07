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
        NotAvailableWindow,
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

    using Error = std::optional<std::error_code> ;
    constexpr std::nullopt_t Succeeded = std::nullopt;


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