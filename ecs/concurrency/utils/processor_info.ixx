module;
#include <span>
export module nx.concurrency.utils.processor_info;
import nx.concurrency.error_code;

export namespace nx {
    enum CoreEfficiencyClass {
        High,
        Low
    };

    nx::Result<std::span<uint64_t>> GetCoreMask(CoreEfficiencyClass)noexcept;

    nx::Error SetCurrentThreadAffinity(uint64_t) noexcept;

}



