module;
#include <expected>
#include <optional>
#include <system_error>
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif


export module nx.core.error_code;


namespace nx
{
    export
    enum class CoreErrc
    {
        Success = 0,
        OutOfBounds,
        BufferOverflow,
        UnknownError = -1,
    };


    class CoreCategory : public std::error_category
    {
    public:
        [[nodiscard]] const char* name() const noexcept override
        {
            return  "co Core";
        };
        [[nodiscard]] std::string message(int code) const override
        {
            switch (static_cast<CoreErrc>(code))
            {
            case CoreErrc::Success:
                return "Success";
            case CoreErrc::OutOfBounds:
                return "Out of bounds";
            case CoreErrc::UnknownError:
            default:
                return "Unknown";
            }
        };
    };

    const std::error_category& gCoreCategory() {
        static CoreCategory c;
        return c;
    }

    export
    std::error_code make_error_code(CoreErrc e) {
        return std::error_code{static_cast<int>(e), gCoreCategory()};
    }

    export
    using Error = std::optional<std::error_code> ;
    export
    constexpr std::nullopt_t  NoError = std::nullopt;


    export
    template<typename T>
    using Result = std::expected<T,std::error_code>;

    export
    auto UnExpected(std::error_code ec)
    {
        return std::unexpected(ec);
    }

#if defined(WIN32)
    export
    std::error_code make_system_error()
    {
        return std::error_code(GetLastError(), std::system_category());
    }

#endif


}

namespace std {
    template <>
    struct is_error_code_enum<nx::CoreCategory> : true_type {}; // 启用枚举到 error_code 的隐式转换
}