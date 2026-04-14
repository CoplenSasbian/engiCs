#include "render_pass.h"
#include "allocator_callbacks.h"
#include "resource.h"
#include "defs.h"

nx::Result<nx::CommonPtr<nx::RhiRenderPass>> nx::VkRenderPass::DeviceCreate(vk::Device& d, const RenderPassCreateInfo& tInfo) noexcept
{
    std::byte mem[4096];
    std::pmr::monotonic_buffer_resource resource(mem, sizeof(mem));

    std::pmr::vector<vk::AttachmentDescription> attachments{ &resource };
    std::pmr::vector<vk::SubpassDescription> subpasses{ &resource };

    attachments.reserve(tInfo.attachments.size());
    subpasses.reserve(tInfo.subpasses.size());

    for (auto& attachment : tInfo.attachments)
    {
        vk::AttachmentDescription desc;

        auto image = static_cast<VkImage*>(attachment.texture);

        desc.setInitialLayout(ToVulkan(attachment.initialLayout))
            .setFinalLayout(ToVulkan(attachment.finalLayout))
            .setFormat(ToVulkan(image->GetImageInfo().format))
            .setLoadOp(ToVulkan(attachment.loadOp))
            .setStoreOp(ToVulkan(attachment.storeOp));

        if (attachment.type == EAttachmentType::DepthStencil) {
            desc.setStencilLoadOp(ToVulkan(attachment.stencilOp.loadOp))
                .setStencilStoreOp(ToVulkan(attachment.stencilOp.storeOp));
        }
        attachments.push_back(desc);
    }

    std::pmr::list<std::pmr::vector<vk::AttachmentReference>> refs{ &resource };

    auto pushRef = [](auto& targetRefs, const auto& attachmentDesc) {
        for (const auto& attachment : attachmentDesc)
        {
            vk::AttachmentReference ref;
            ref.setAttachment(attachment.attachmentIndex);
            ref.setLayout(ToVulkan(attachment.layout));
            targetRefs.push_back(ref);
        }
        };


    auto& depthRefs = refs.emplace_back();
    depthRefs.reserve(tInfo.subpasses.size());

    for (auto& subpass_desc : tInfo.subpasses)
    {
        auto& subDesc = subpasses.emplace_back();

        auto& colorRefs = refs.emplace_back();
        colorRefs.reserve(subpass_desc.colorAttachmentIndices.size());
        pushRef(colorRefs, subpass_desc.colorAttachmentIndices);
        subDesc.setColorAttachments(colorRefs);

        auto& inputRefs = refs.emplace_back();
        inputRefs.reserve(subpass_desc.inputAttachmentIndices.size());
        pushRef(inputRefs, subpass_desc.inputAttachmentIndices);
        subDesc.setInputAttachments(inputRefs);

        auto& resolveRefs = refs.emplace_back();
        resolveRefs.reserve(subpass_desc.resolveAttachmentIndices.size());
        pushRef(resolveRefs, subpass_desc.resolveAttachmentIndices);
        subDesc.setResolveAttachments(resolveRefs);

        if (subpass_desc.depthStencilAttachment.attachmentIndex != std::numeric_limits<uint32_t>::max())
        {
            auto& depth_ref = depthRefs.emplace_back();
            depth_ref.setAttachment(subpass_desc.depthStencilAttachment.attachmentIndex);
            depth_ref.setLayout(ToVulkan(subpass_desc.depthStencilAttachment.layout));
            subDesc.setPDepthStencilAttachment(&depth_ref);
        }
    }


    vk::RenderPassCreateInfo info;
    info.setAttachments(attachments)
        .setSubpasses(subpasses);


    vk::RenderPass renderPass;
    VK_UNEXPECT_ON_ERROR(d.createRenderPass(&info, GetVulkanAllocatorCallbacks(), &renderPass));

    return MakeCommonPtr<VkRenderPass>(d, std::move(renderPass));
}

nx::VkRenderPass::VkRenderPass(vk::Device& device, vk::RenderPass&& renderPass)
	: m_device(device), m_renderPass(std::move(renderPass))
{
}

nx::VkRenderPass::~VkRenderPass()
{
	if (m_renderPass)
		m_device.destroyRenderPass(m_renderPass, GetVulkanAllocatorCallbacks());
}
