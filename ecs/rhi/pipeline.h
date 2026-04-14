#pragma once

#include"shader.h"
#include "defs.h"
namespace nx
{

    struct BlendAttachmentState {
        bool blendEnable = false;

        EBlendOp colorBlendOp = EBlendOp::eAdd;
        EBlendFactor srcColorBlendFactor = EBlendFactor::eOne;
        EBlendFactor dstColorBlendFactor = EBlendFactor::eZero;

        EBlendOp alphaBlendOp = EBlendOp::eAdd;
        EBlendFactor srcAlphaBlendFactor = EBlendFactor::eOne;
        EBlendFactor dstAlphaBlendFactor = EBlendFactor::eZero;

        EColorWriteMaskFlags writeMask = EColorWriteMaskFlagBits::eAll;
    };

    struct MultisampleStateDesc {
        ESampleCountFlags sampleCount = ESampleCountFlagBits::e1;
        uint32_t sampleMask = 0xFFFFFFFF;
    };


    struct RasterizerStateDesc {
        EPolygonMode polygonMode = EPolygonMode::eFill;
        ECullMode cullMode = ECullMode::eBack;
        EFrontFace frontFace = EFrontFace::eCounterClockwise;

        bool depthClampEnable = false;
        bool rasterizerDiscardEnable = false;

        bool depthBiasEnable = false;
        float depthBiasConstant = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlopeScale = 0.0f;

        float lineWidth = 1.0f;
    };

    struct BlendStateDesc {
        bool alphaToCoverageEnable = false;
        bool independentBlendEnable = false;

        std::vector<BlendAttachmentState> attachments;
    };

    struct StencilOpState {
        EStencilOp failOp = EStencilOp::eKeep;
        EStencilOp depthFailOp = EStencilOp::eKeep;
        EStencilOp passOp = EStencilOp::eKeep;
        ECompareOp compareFunc = ECompareOp::eAlways;
    };
    struct DepthStencilStateDesc {
        bool depthTestEnable = false;
        bool depthWriteEnable = false;
        ECompareOp depthCompareOp = ECompareOp::eLess;

        bool stencilTestEnable = false;
        StencilOpState frontFace;
        StencilOpState backFace;

        uint8_t stencilReadMask = 0xFF;
        uint8_t stencilWriteMask = 0xFF;
    };


    struct PipelineCreateInfo {
        std::vector<RhiShader*> shaders;

        RasterizerStateDesc   rasterizer;
        MultisampleStateDesc  multisample;
        BlendStateDesc        blend;
        DepthStencilStateDesc depthStencil;
        std::vector<EPixelFormat> renderTargetFormats;
        EPixelFormat depthStencilFormat = EPixelFormat::eUnknown;




    };

    class RhiPipeline
    {
    public:

    };
}
