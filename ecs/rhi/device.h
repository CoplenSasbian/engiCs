#pragma once
#include "core/memory/memory.h"

#include "core/error_code.h"
#include "queue.h"
#include "shader.h"
#include "surface.h"
#include "resource.h"
#include "render_pass.h"
#include "swap_chain.h"

namespace nx {

    class RhiDevice {
    public:
        RhiDevice() = default;
        virtual ~RhiDevice() = default;
        virtual Error Initialize(RhiSurface* = nullptr)noexcept=0;
        virtual RhiQueue* GetQueue(EQueueType type)noexcept = 0;

        virtual Result<CommonPtr<RhiSwapChain>> CreateSwapChain(RhiSurface*) noexcept = 0;
		virtual Result<CommonPtr<RhiImage>> CreateImage(const ImageCreateInfo&) noexcept = 0;
		virtual Result<CommonPtr<RhiBuffer>> CreateBuffer(const BufferCreateInfo&) noexcept = 0;
        virtual Result<CommonPtr<RhiRenderPass>> CreateRenderPass(const RenderPassCreateInfo&) noexcept = 0;
    };


} // namespace nx