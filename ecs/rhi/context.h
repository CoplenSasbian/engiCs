#pragma once
#include "core/memory/memory.h"

#include "core/error_code.h"
#include "shader.h"
#include "surface.h"
#include "resource.h"
#include "render_pass.h"
#include "swap_chain.h"
#include "command_buffer.h"
#include "pipeline.h"
namespace nx {

    class RhiContext {
    public:
        RhiContext() = default;
        virtual ~RhiContext() = default;
        virtual Error Initialize(RhiSurface*)noexcept=0;

        virtual Result<CommonPtr<RhiSwapChain>> CreateSwapChain(RhiSurface*) noexcept = 0;
		virtual Result<CommonPtr<RhiImage>> CreateImage(const ImageCreateInfo&) noexcept = 0;
		virtual Result<CommonPtr<RhiBuffer>> CreateBuffer(const BufferCreateInfo&) noexcept = 0;
        virtual Result<CommonPtr<RhiRenderPass>> CreateRenderPass(const RenderPassCreateInfo&) noexcept = 0;
        virtual Result<CommonPtr<RhiCmdBuffer>> CreateCommandBuffer(EQueueType type)noexcept = 0;
        virtual Result<CommonPtr<RhiPipeline>> CreatePipeline(const PipelineCreateInfo&) noexcept = 0;

        virtual Result<CommonPtr<RhiShader>> CreateShader(const ShaderCreateInfo&) noexcept = 0;

    };


} // namespace nx