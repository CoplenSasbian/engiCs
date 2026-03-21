#pragma once
#include "defs.h"
namespace nx {
    class RhiQueue {
    public:
        using enum EQueueTypeBits;
        virtual ~RhiQueue() = default;
        virtual EQueueType GetType() noexcept = 0;
    };
}