module;
#include <system_error>
#include <boost/exception/exception.hpp>

export module nx.concurrency.error_code;
export  import nx.core.error_code;

namespace nx
{
    export
    enum class ConcurrencyErrc
    {
        Success = 0,
        QueueFull,
        LoopStopped,
        InvalidThreadToken,
        InvalidThreadType,
    };
    export
    class ConcurrencyCategory : public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return  "ecs Concurrency";
        };
        std::string message(int code) const override
        {
            switch (static_cast<ConcurrencyErrc>(code))
            {
                case ConcurrencyErrc::Success:
                    return "Success";
                case ConcurrencyErrc::QueueFull:
                    return "Queue Full";
                case ConcurrencyErrc::LoopStopped:
                    return "Event loop has stopped";
                case ConcurrencyErrc::InvalidThreadToken:
                    return "Invalid thread token";
                case ConcurrencyErrc::InvalidThreadType:
                    return "Invalid thread type";
                default:
                    return "Unknown";
            }
        };
    };

    const std::error_category& gConcurrencyCategory() {
        static ConcurrencyCategory c;
        return c;
    }

    export
    std::error_code make_error_code(ConcurrencyErrc e) {
        return std::error_code(static_cast<int>(e), gConcurrencyCategory());
    }

}
namespace std {
    export
    template <>
    struct is_error_code_enum<nx::ConcurrencyCategory> : true_type {}; // 启用枚举到 error_code 的隐式转换
}