#pragma once
#include <memory>
#include <span>
#include "rhi/queue.h"
#include "rhi/shader.h"

namespace nx {



    class Device {
    public:
        virtual ~Device() = default;
        virtual void Initialize()=0;
        virtual Queue* GetQueue(QueueType type) = 0;

        virtual std::shared_ptr<Shader> CreateShader(std::span<std::byte> data)=0;

    };


} // namespace nx