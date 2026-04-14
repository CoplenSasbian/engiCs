#pragma once
#include "core/memory/memory.h"
#include "context.h"
#include "surface.h"

namespace nx
{
    class RhiInstance
    {
    public:
        RhiInstance() = default;
        virtual ~RhiInstance() = default;
        virtual Error  Initialize(const std::string& appName)noexcept = 0;
        virtual Result<CommonPtr<RhiSurface>> CreateSurface(void* nativeWindowHandle) noexcept = 0;
        virtual Result<CommonPtr<RhiContext>> CreateContext() noexcept = 0;
    };
}



