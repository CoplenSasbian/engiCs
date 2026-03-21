#include "render_pass.h"


 nx::AttachmentDesc  nx::AttachmentDesc::ColorAttachment(RhiImage* texture, EImageLayout initLayout,
        EImageLayout finalLayout, ELoadOp loadOp, EStoreOp storeOp, Color clearColor)
{
     nx::AttachmentDesc desc{};
     desc.type = EAttachmentType::Color;
     desc.texture = texture;
     desc.initialLayout = initLayout;
     desc.finalLayout = finalLayout;
     desc.loadOp = loadOp;
     desc.storeOp = storeOp;
     desc.clearValue.color[0] = clearColor.f.r;
     desc.clearValue.color[1] = clearColor.f.g;
     desc.clearValue.color[2] = clearColor.f.b;
     desc.clearValue.color[3] = clearColor.f.a;
     return desc;
}

nx::AttachmentDesc  nx::AttachmentDesc::DepthAttachment(RhiImage* texture, EImageLayout initLayout,
    EImageLayout finalLayout, ELoadOp loadOp, EStoreOp storeOp, float clearDepth)
{
     nx::AttachmentDesc desc{};
     desc.type = EAttachmentType::Depth;
     desc.texture = texture;
     desc.initialLayout = initLayout;
     desc.finalLayout = finalLayout;
     desc.loadOp = loadOp;
     desc.storeOp = storeOp;
     desc.clearValue.depthStencil.depth = clearDepth;
     return desc;
}

nx::AttachmentDesc  nx::AttachmentDesc::DepthStencilAttachment(RhiImage* texture, EImageLayout initLayout,
    EImageLayout finalLayout, ELoadOp loadOp, EStoreOp storeOp, float clearDepth, uint8_t clearStencil)
{
     nx::AttachmentDesc desc{};
     desc.type = EAttachmentType::Depth;
     desc.texture = texture;
     desc.initialLayout = initLayout;
     desc.finalLayout = finalLayout;
     desc.loadOp = loadOp;
     desc.storeOp = storeOp;
     desc.clearValue.depthStencil.depth = clearDepth;
     desc.clearValue.depthStencil.stencil = clearStencil;
     return desc;
}

bool nx::AttachmentDesc::operator==(const AttachmentDesc& other) const
{
    return texture == other.texture && type == other.type && initialLayout == other.initialLayout && finalLayout == other.finalLayout && loadOp == other.loadOp && storeOp == other.storeOp
        && clearValueEquals(other.clearValue);
}

bool nx::AttachmentDesc::clearValueEquals(const ClearValue& other, float epsilon) const
{
    switch (type) {
    case EAttachmentType::Color:
        return std::equal(
            clearValue.color, clearValue.color + 4, other.color,
            [epsilon](float a, float b) { return std::abs(a - b) <= epsilon; }
        );

    case EAttachmentType::Depth:
        return std::abs(clearValue.depthStencil.depth - other.depthStencil.depth) <= epsilon;

    case EAttachmentType::DepthStencil:
        return (std::abs(clearValue.depthStencil.depth - other.depthStencil.depth) <= epsilon)
            && (clearValue.depthStencil.stencil == other.depthStencil.stencil); // 模板精确比较

    default:
        return false;
    }
}



namespace nx {

uint32_t RenderPassBuilder::AddAttachment(std::string name, const AttachmentDesc& desc) {
    if (m_nameToIndex.find(name) != m_nameToIndex.end()) {
        throw std::invalid_argument(std::format("Attachment name '{}' already registered", name));
    }
    uint32_t idx = static_cast<uint32_t>(m_attachments.size());
    m_nameToIndex[std::move(name)] = idx;
    m_attachments.push_back(desc);
    return idx;
}

uint32_t RenderPassBuilder::AddAttachment(const AttachmentDesc& desc) {
    m_attachments.push_back(desc);
    return static_cast<uint32_t>(m_attachments.size() - 1);
}

uint32_t RenderPassBuilder::AddColorAttachment(
    std::string name, RhiImage* texture, EImageLayout initLayout, EImageLayout finalLayout,
    ELoadOp loadOp, EStoreOp storeOp, Color clearColor)
{
    return AddAttachment(std::move(name),
        AttachmentDesc::ColorAttachment(texture, initLayout, finalLayout, loadOp, storeOp, clearColor));
}

uint32_t RenderPassBuilder::AddDepthAttachment(
    std::string name, RhiImage* texture, EImageLayout initLayout, EImageLayout finalLayout,
    ELoadOp loadOp, EStoreOp storeOp, float clearDepth)
{
    return AddAttachment(std::move(name),
        AttachmentDesc::DepthAttachment(texture, initLayout, finalLayout, loadOp, storeOp, clearDepth));
}

uint32_t RenderPassBuilder::AddDepthStencilAttachment(
    std::string name, RhiImage* texture, EImageLayout initLayout, EImageLayout finalLayout,
    ELoadOp loadOp, EStoreOp storeOp, float clearDepth, uint8_t clearStencil)
{
    return AddAttachment(std::move(name),
        AttachmentDesc::DepthStencilAttachment(texture, initLayout, finalLayout,
            loadOp, storeOp, clearDepth, clearStencil));
}

RenderPassBuilder& RenderPassBuilder::BeginSubpass() {
    assert(!m_activeSubpass && "Must call EndSubpass() before starting new subpass");
    m_activeSubpass.emplace();
    m_activeSubpass->depthStencilAttachment = {UNUSED_ATTACHMENT, EImageLayout::eUndefined};
    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddColorAttachmentRef(uint32_t idx, EImageLayout layout) {
    assert(m_activeSubpass); m_activeSubpass->colorAttachmentIndices.push_back({idx, layout}); return *this;
}
RenderPassBuilder& RenderPassBuilder::AddInputAttachmentRef(uint32_t idx, EImageLayout layout) {
    assert(m_activeSubpass); m_activeSubpass->inputAttachmentIndices.push_back({idx, layout}); return *this;
}
RenderPassBuilder& RenderPassBuilder::AddResolveAttachmentRef(uint32_t idx, EImageLayout layout) {
    assert(m_activeSubpass); m_activeSubpass->resolveAttachmentIndices.push_back({idx, layout}); return *this;
}
RenderPassBuilder& RenderPassBuilder::SetDepthStencilAttachmentRef(uint32_t idx, EImageLayout layout) {
    assert(m_activeSubpass); m_activeSubpass->depthStencilAttachment = {idx, layout}; return *this;
}

RenderPassBuilder& RenderPassBuilder::AddColorAttachmentRef(std::string_view name, EImageLayout layout) {
    return AddColorAttachmentRef(resolveAttachmentIndex(name), layout);
}
RenderPassBuilder& RenderPassBuilder::AddInputAttachmentRef(std::string_view name, EImageLayout layout) {
    return AddInputAttachmentRef(resolveAttachmentIndex(name), layout);
}
RenderPassBuilder& RenderPassBuilder::AddResolveAttachmentRef(std::string_view name, EImageLayout layout) {
    return AddResolveAttachmentRef(resolveAttachmentIndex(name), layout);
}
RenderPassBuilder& RenderPassBuilder::SetDepthStencilAttachmentRef(std::string_view name, EImageLayout layout) {
    return SetDepthStencilAttachmentRef(resolveAttachmentIndex(name), layout);
}

RenderPassBuilder& RenderPassBuilder::EndSubpass() {
    assert(m_activeSubpass && "No active subpass to end");
    assert((m_activeSubpass->resolveAttachmentIndices.empty() ||
            m_activeSubpass->resolveAttachmentIndices.size() == m_activeSubpass->colorAttachmentIndices.size()) &&
           "Resolve attachments count must match color attachments count");
    m_subpasses.push_back(std::move(*m_activeSubpass));
    m_activeSubpass.reset();
    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubpassDependency(
    uint32_t srcSubpass, uint32_t dstSubpass,
    EImageLayout srcLayout, EImageLayout dstLayout,
    EImageAspectFlagBits aspect)
{
    m_dependencies.push_back({srcSubpass, dstSubpass, srcLayout, dstLayout, aspect});
    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubpassChainDependency(
    EImageLayout srcLayout, EImageLayout dstLayout, EImageAspectFlagBits aspect)
{
    if (m_subpasses.size() < 2) return *this;
    uint32_t src = static_cast<uint32_t>(m_subpasses.size() - 2);
    uint32_t dst = static_cast<uint32_t>(m_subpasses.size() - 1);
    return AddSubpassDependency(src, dst, srcLayout, dstLayout, aspect);
}

RenderPassBuilder& RenderPassBuilder::AddExternalToFirstSubpass(
    EImageLayout initialLayout, EImageAspectFlagBits aspect)
{
    if (m_subpasses.empty()) return *this;
    return AddSubpassDependency(EXTERNAL_SUBPASS, 0, EImageLayout::eUndefined, initialLayout, aspect);
}

RenderPassBuilder& RenderPassBuilder::AddLastSubpassToExternal(
    EImageLayout finalLayout, EImageAspectFlagBits aspect)
{
    if (m_subpasses.empty()) return *this;
    return AddSubpassDependency(
        static_cast<uint32_t>(m_subpasses.size() - 1), EXTERNAL_SUBPASS, finalLayout, EImageLayout::eUndefined, aspect);
}

RenderPassCreateInfo RenderPassBuilder::Build() {
    assert(!m_activeSubpass && "Forgot to call EndSubpass()!");

    RenderPassCreateInfo info{
        .attachments = std::move(m_attachments),
        .subpasses = std::move(m_subpasses),
        .dependencies = std::move(m_dependencies)
    };

    m_attachments.clear();
    m_subpasses.clear();
    m_dependencies.clear();
    m_nameToIndex.clear();

    return info;
}

const std::unordered_map<std::string, uint32_t>& RenderPassBuilder::GetAttachmentNames() const noexcept {
    return m_nameToIndex;
}

uint32_t RenderPassBuilder::resolveAttachmentIndex(std::string_view name) const {
    auto it = m_nameToIndex.find(std::string(name));
    assert(it != m_nameToIndex.end() && "Attachment name not found");
    return it->second;
}

} // namespace nx
