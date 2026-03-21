#pragma once

#include "command_buffer.h"
#include "core/error_code.h"
#include  "queue.h"
namespace nx
{
    class RhiCommandPool
    {
    public:
        virtual ~RhiCommandPool()=default;
        virtual Error Initialize(RhiQueue* ) noexcept = 0;
        virtual RhiCommandBuffer* GetBuffer()noexcept = 0;
        virtual Error Reset()noexcept = 0;
        virtual Error Submit()noexcept = 0;
    };
}
