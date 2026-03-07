#pragma once
#include <span>
#include "core/error_code.h"

namespace nx {
    enum CoreEfficiencyClass {
        High,
        Low
    };

    nx::Result<std::span<uint64_t>> GetCoreMask(CoreEfficiencyClass)noexcept;

    nx::Error SetCurrentThreadAffinity(uint64_t) noexcept;

}



