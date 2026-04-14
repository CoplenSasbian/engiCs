#pragma once
#include "defs.h"
#include <vector>
#include "resource.h"
#include <unordered_map>
namespace nx {

	struct AttachmentDesc {
		RhiImage* texture;
		EAttachmentType type;
		EImageLayout initialLayout;
		EImageLayout finalLayout;
		ELoadOp loadOp;
		EStoreOp storeOp;
	
		struct StencilOp{
			ELoadOp loadOp;
			EStoreOp storeOp;
		}stencilOp;

		union ClearValue {
			float color[4];
			struct { float depth; uint32_t stencil; } depthStencil;
		} clearValue;

		static AttachmentDesc  ColorAttachment(RhiImage* texture,
			EImageLayout initLayout = EImageLayout::eCommon,
			EImageLayout finalLayout = EImageLayout::eCommon,
			ELoadOp loadOp = ELoadOp::eDiscard,
			EStoreOp storeOp = EStoreOp::eDiscard,
			Color clearColor = { 0.f,0.f,0.f,0.f }
		);

		static AttachmentDesc DepthAttachment(RhiImage* texture,
			EImageLayout initLayout = EImageLayout::eCommon,
			EImageLayout finalLayout = EImageLayout::eCommon,
			ELoadOp loadOp = ELoadOp::eDiscard,
			EStoreOp storeOp = EStoreOp::eDiscard,
			float clearDepth = 0.f);

		static AttachmentDesc DepthStencilAttachment(RhiImage* texture,
			EImageLayout initLayout = EImageLayout::eCommon,
			EImageLayout finalLayout = EImageLayout::eCommon,
			ELoadOp loadOp = ELoadOp::eDiscard,
			EStoreOp storeOp = EStoreOp::eDiscard,
			float clearDepth = 0.f,
			uint8_t clearStencil = 0.f);
	

		bool operator==(const AttachmentDesc& other) const;

		bool clearValueEquals(const ClearValue& other, float epsilon = 1e-6f) const;
	};

	struct  AttachmentRef
	{
		uint32_t attachmentIndex;
		EImageLayout layout;
	};

	struct SubpassDesc{
		std::vector<AttachmentRef> colorAttachmentIndices;
		std::vector<AttachmentRef> inputAttachmentIndices;
		std::vector<AttachmentRef> resolveAttachmentIndices;
		AttachmentRef depthStencilAttachment;
	};

	struct RenderPassDependency {
		static constexpr  auto EXTERNAL_PASS = 0xffffffff;
		uint32_t srcSubpass;
		uint32_t dstSubpass;
		EImageLayout srcLayout;
		EImageLayout dstLayout;
		EImageAspectFlagBits aspect;
	};

	struct RenderPassCreateInfo {
		std::vector< AttachmentDesc> attachments;
		std::vector< SubpassDesc> subpasses;
		std::vector<RenderPassDependency> dependencies;
	};


class RenderPassBuilder {
public:
    static constexpr uint32_t UNUSED_ATTACHMENT = ~0u;
    static constexpr uint32_t EXTERNAL_SUBPASS  = ~0u;

    uint32_t AddAttachment(std::string name, const AttachmentDesc& desc);
    uint32_t AddAttachment(const AttachmentDesc& desc); // 无名附件

    uint32_t AddColorAttachment(
        std::string name,
        RhiImage* texture,
        EImageLayout initLayout = EImageLayout::eCommon,
        EImageLayout finalLayout = EImageLayout::eCommon,
        ELoadOp loadOp = ELoadOp::eClear,
        EStoreOp storeOp = EStoreOp::eStore,
        Color clearColor = {0.f, 0.f, 0.f, 0.f}
    );

    uint32_t AddDepthAttachment(
        std::string name,
        RhiImage* texture,
        EImageLayout initLayout = EImageLayout::eCommon,
        EImageLayout finalLayout = EImageLayout::eCommon,
        ELoadOp loadOp = ELoadOp::eClear,
        EStoreOp storeOp = EStoreOp::eStore,
        float clearDepth = 1.f
    );

    uint32_t AddDepthStencilAttachment(
        std::string name,
        RhiImage* texture,
        EImageLayout initLayout = EImageLayout::eCommon,
        EImageLayout finalLayout = EImageLayout::eCommon,
        ELoadOp loadOp = ELoadOp::eClear,
        EStoreOp storeOp = EStoreOp::eStore,
        float clearDepth = 1.f,
        uint8_t clearStencil = 0
    );

    RenderPassBuilder& BeginSubpass();

    RenderPassBuilder& AddColorAttachmentRef(uint32_t idx, EImageLayout layout);
    RenderPassBuilder& AddInputAttachmentRef(uint32_t idx, EImageLayout layout);
    RenderPassBuilder& AddResolveAttachmentRef(uint32_t idx, EImageLayout layout);
    RenderPassBuilder& SetDepthStencilAttachmentRef(uint32_t idx, EImageLayout layout);

    RenderPassBuilder& AddColorAttachmentRef(std::string_view name, EImageLayout layout);
    RenderPassBuilder& AddInputAttachmentRef(std::string_view name, EImageLayout layout);
    RenderPassBuilder& AddResolveAttachmentRef(std::string_view name, EImageLayout layout);
    RenderPassBuilder& SetDepthStencilAttachmentRef(std::string_view name, EImageLayout layout);

    RenderPassBuilder& EndSubpass();

    RenderPassBuilder& AddSubpassDependency(
        uint32_t srcSubpass,
        uint32_t dstSubpass,
        EImageLayout srcLayout,
        EImageLayout dstLayout,
        EImageAspectFlagBits aspect
    );

    RenderPassBuilder& AddSubpassChainDependency(
        EImageLayout srcLayout,
        EImageLayout dstLayout,
        EImageAspectFlagBits aspect
    );

    RenderPassBuilder& AddExternalToFirstSubpass(
        EImageLayout initialLayout,
        EImageAspectFlagBits aspect
    );

    RenderPassBuilder& AddLastSubpassToExternal(
        EImageLayout finalLayout,
        EImageAspectFlagBits aspect
    );

    RenderPassCreateInfo Build();
    const std::unordered_map<std::string, uint32_t>& GetAttachmentNames() const noexcept;

private:
   uint32_t resolveAttachmentIndex(std::string_view name) const;

    std::vector<AttachmentDesc> m_attachments;
    std::vector<SubpassDesc> m_subpasses;
    std::vector<RenderPassDependency> m_dependencies;
    std::optional<SubpassDesc> m_activeSubpass;
    std::unordered_map<std::string, uint32_t> m_nameToIndex;
};




	class RhiRenderPass {
	public:
		virtual ~RhiRenderPass() = default;
		RhiRenderPass() =default;
		
	};
}