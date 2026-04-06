#include "render_queue.h"
#include "renderer/renderer.h"
#include "material/material.h"
#include "shader/shader.h"
#include "hash.h"
#include "graphics/render_settings/render_settings.h"
#include "graphics_backend_api.h"
#include "graphics_buffer/ring_buffer.h"
#include "texture/texture.h"
#include "drawable_geometry/drawable_geometry.h"
#include "global_constants.h"
#include "developer_console/developer_console.h"
#include "graphics_buffer/graphics_buffer_view.h"
#include "editor/profiler/profiler.h"
#include "debug.h"

bool RenderQueue::EnableFrustumCulling = true;
bool RenderQueue::FreezeFrustumCulling = false;

std::mutex RenderQueue::s_PermanentMatricesUpdatesMutex;
std::shared_mutex RenderQueue::s_PermanentMatricesBufferRecreateMutex;
std::vector<std::pair<Matrix4x4, uint32_t>> RenderQueue::s_PermanentMatricesUpdates;
std::shared_ptr<GraphicsBuffer> RenderQueue::s_PermanentMatricesBuffer;
std::shared_ptr<GraphicsBufferView> RenderQueue::s_PermanentMatricesBufferView;

std::deque<uint32_t> RenderQueue::s_FreeMatricesBufferEntries;
std::mutex RenderQueue::s_FreeMatricesBufferEntriesMutex;
uint32_t RenderQueue::s_MatricesBufferCapacity = 1024;

namespace RenderQueueLocal
{
    constexpr uint32_t k_MatricesBufferElementSize = 2 * sizeof(Matrix4x4);

    std::size_t GetDrawCallInstancingHash(const DrawCallInfo &drawCallInfo)
    {
        const std::size_t materialHash = std::hash<const Material *> {}(drawCallInfo.Material);
        const std::size_t geometryHash = std::hash<const DrawableGeometry *> {}(drawCallInfo.Geometry);
        return Hash::Combine(materialHash, geometryHash);
    }

    void SortDrawCalls(DrawCallSortMode sortMode, const Matrix4x4& viewProjectionMatrix, std::vector<DrawCallInfo>& outDrawCalls)
    {
        Profiler::Marker _("RenderQueue::SortDrawCalls");

        const Matrix4x4 invViewProjection = viewProjectionMatrix.Invert();
        const Vector4 cameraDirection = invViewProjection * Vector4(0, 0, 1, 0);

        if (sortMode != DrawCallSortMode::NO_SORTING)
            std::sort(outDrawCalls.begin(), outDrawCalls.end(), DrawCallComparer {sortMode, cameraDirection});
    }

    int GetEntryFromBufferView(const std::shared_ptr<GraphicsBufferView>& view)
    {
        if (!view || !view->GetBuffer())
            return -1;

        const GraphicsBackendBufferViewDescriptor& viewDescriptor = view->GetDescriptor();
        return viewDescriptor.Offset / k_MatricesBufferElementSize;
    }
}

RenderQueue::RenderQueue() :
	m_PreviousMaterial(nullptr),
	m_PreviousVertexAttributesHash(0),
	m_PreviousPrimitiveType(PrimitiveType::LINES)
{
    DeveloperConsole::AddBoolCommand(L"FrustumCulling.Enabled", &EnableFrustumCulling);
    DeveloperConsole::AddBoolCommand(L"FrustumCulling.Freeze", &FreezeFrustumCulling);

    if (!s_PermanentMatricesBuffer)
	    CreatePermanentMatricesBuffer();
}

void RenderQueue::Prepare(const Matrix4x4& viewProjectionMatrix, const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& renderSettings)
{
    Profiler::Marker _("RenderQueue::Prepare");

    Clear();

    if (!FreezeFrustumCulling)
        m_Frustum = Frustum(viewProjectionMatrix);

    SetupDrawCalls(renderers, renderSettings, m_Frustum);
	BatchDrawCalls();
    RenderQueueLocal::SortDrawCalls(renderSettings.Sorting, viewProjectionMatrix, m_DrawCalls);
}

void RenderQueue::Prepare(const Matrix4x4& viewProjectionMatrix, const std::vector<Item>& items, const RenderSettings& renderSettings)
{
    Profiler::Marker _("RenderQueue::Prepare");

    Clear();

    if (!FreezeFrustumCulling)
        m_Frustum = Frustum(viewProjectionMatrix);

    SetupDrawCalls(items, renderSettings, m_Frustum);
    BatchDrawCalls();
    RenderQueueLocal::SortDrawCalls(renderSettings.Sorting, viewProjectionMatrix, m_DrawCalls);
}

void RenderQueue::Clear()
{
    Profiler::Marker _("RenderQueue::Clear");

    m_DrawCalls.clear();
    m_PreviousMaterial = nullptr;
    m_PreviousVertexAttributesHash = 0;
    m_PreviousPrimitiveType = PrimitiveType::LINES;

    m_InstancedMatricesEntries.clear();
    m_InstancedMatricesEntriesCounts.clear();

    m_TemporaryMatrices.clear();
}

bool RenderQueue::IsEmpty() const
{
    return m_DrawCalls.empty();
}

const std::vector<DrawCallInfo>& RenderQueue::GetDrawCalls() const
{
    return m_DrawCalls;
}

void RenderQueue::Draw()
{
    if (!m_InstancedMatricesEntries.empty())
	    m_InstancedMatricesEntriesBuffer->SetData(m_InstancedMatricesEntries.data(), 0, m_InstancedMatricesEntries.size() * sizeof(uint32_t));

    if (!m_TemporaryMatrices.empty())
        m_TemporaryMatricesBuffer->SetData(m_TemporaryMatrices.data(), 0, m_TemporaryMatrices.size() * sizeof(Matrix4x4));

    if (!s_PermanentMatricesUpdates.empty())
    {
        for (const std::pair<Matrix4x4, uint32_t>& pair : s_PermanentMatricesUpdates)
        {
            Matrix4x4 matrices[2];
            matrices[0] = pair.first;
            matrices[1] = pair.first.Invert().Transpose();
            s_PermanentMatricesBuffer->SetData(&matrices[0], pair.second * sizeof(matrices), sizeof(matrices));
        }
        s_PermanentMatricesUpdates.clear();
    }

    for (const DrawCallInfo& drawCall : m_DrawCalls)
    {
        const GraphicsBackendGeometry& geom = drawCall.Geometry->GetGraphicsBackendGeometry();
        const PrimitiveType primitiveType = drawCall.Geometry->GetPrimitiveType();
        const IndicesDataType indicesDataType = drawCall.Geometry->GetIndicesDataType();
        const int elementsCount = drawCall.Geometry->GetElementsCount();
        const bool hasIndices = drawCall.Geometry->HasIndexes();

        SetupMatrices(drawCall);
        SetupShaderPass(drawCall.Material, drawCall.Geometry->GetVertexAttributes(), primitiveType, drawCall.StencilValue);

        if (drawCall.Instanced)
        {
            const int instanceCount = drawCall.MatricesBufferViews.size();
            if (hasIndices)
                GraphicsBackend::Current()->DrawElementsInstanced(geom, primitiveType, elementsCount, indicesDataType, instanceCount);
            else
                GraphicsBackend::Current()->DrawArraysInstanced(geom, primitiveType, 0, elementsCount, instanceCount);
        }
        else
        {
            if (hasIndices)
                GraphicsBackend::Current()->DrawElements(geom, primitiveType, elementsCount, indicesDataType);
            else
                GraphicsBackend::Current()->DrawArrays(geom, primitiveType, 0, elementsCount);
        }
    }
}

void RenderQueue::SetupDrawCalls(const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& settings, const Frustum& frustum)
{
    Profiler::Marker _("RenderQueue::SetupDrawCalls");

    m_DrawCalls.reserve(renderers.size());

    CheckMatricesBufferSize();

    std::shared_lock lock(s_PermanentMatricesBufferRecreateMutex);

    for (const std::shared_ptr<Renderer>& renderer : renderers)
    {
        if (!renderer)
            continue;

        const Material* material = settings.OverrideMaterial ? settings.OverrideMaterial.get() : renderer->GetMaterial().get();
        const DrawableGeometry* geometry = renderer->GetGeometry().get();

        if (!geometry || !material)
            continue;

        DrawCallInfo info{};
        info.Geometry = geometry;
        info.Material = material;
        info.AABB = renderer->GetAABB();
        info.CastShadows = renderer->CastShadows;
        info.StencilValue = renderer->StencilValue;

        if (!settings.Filter(info))
            continue;

        if (EnableFrustumCulling && !frustum.IsVisible(renderer->GetAABB(), settings.FrustumCullingPlanesBits))
            continue;

        std::shared_ptr<GraphicsBufferView> matricesBufferView = nullptr;
        {
            std::unique_lock rendererLock(renderer->GetMatricesBufferViewMutex());

            bool matricesBufferViewChanged = false;
            matricesBufferView = renderer->GetMatricesBufferView();
            if (!matricesBufferView || !matricesBufferView->GetBuffer())
            {
            	const int matricesBufferEntry = GetMatricesEntry();
                if (matricesBufferEntry >= 0)
                {
                    const GraphicsBackendBufferViewDescriptor viewDescriptor = GraphicsBackendBufferViewDescriptor::Structured(1, RenderQueueLocal::k_MatricesBufferElementSize, matricesBufferEntry * RenderQueueLocal::k_MatricesBufferElementSize, false);
                    matricesBufferView = std::make_shared<GraphicsBufferView>(s_PermanentMatricesBuffer, viewDescriptor, "RenderQueue/PermanentMatricesBufferSingleView");
                    matricesBufferViewChanged = true;
                }
                else
                    matricesBufferView = nullptr;

                renderer->SetMatricesBufferView(matricesBufferView);
            }

            if (matricesBufferView && (renderer->IsTransformDirty() || matricesBufferViewChanged))
            {
                std::lock_guard<std::mutex> updatesLock(s_PermanentMatricesUpdatesMutex);
                s_PermanentMatricesUpdates.emplace_back(renderer->GetModelMatrix(), RenderQueueLocal::GetEntryFromBufferView(matricesBufferView));
                renderer->SetTransformDirty(false);
            }
        }

        if (matricesBufferView)
        {
            info.MatricesBufferViews.push_back(matricesBufferView);
            m_DrawCalls.push_back(info);
        }
    }
}

void RenderQueue::SetupDrawCalls(const std::vector<Item>& items, const RenderSettings& settings, const Frustum& frustum)
{
    Profiler::Marker _("RenderQueue::SetupDrawCalls");

    m_DrawCalls.reserve(items.size());

    const uint32_t requiredBufferSize = items.size() * RenderQueueLocal::k_MatricesBufferElementSize;
    m_TemporaryMatrices.reserve(items.size() * 2);
    if (!m_TemporaryMatricesBuffer || m_TemporaryMatricesBuffer->GetSize() < requiredBufferSize)
    {
        GraphicsBackendBufferDescriptor descriptor{};
        descriptor.AllowCPUWrites = true;
        descriptor.Size = requiredBufferSize;

        m_TemporaryMatricesBuffer = std::make_shared<GraphicsBuffer>(descriptor, "RenderQueue/TemporaryMatrices");

        const GraphicsBackendBufferViewDescriptor viewDescriptor = GraphicsBackendBufferViewDescriptor::Structured(items.size(), RenderQueueLocal::k_MatricesBufferElementSize, 0, false);
        m_TemporaryMatricesBufferView = std::make_shared<GraphicsBufferView>(m_TemporaryMatricesBuffer, viewDescriptor, "RenderQueue/TemporaryMatricesFullView");
    }

    uint32_t offset = 0;
    for (const Item& item : items)
    {
        const Material* material = settings.OverrideMaterial ? settings.OverrideMaterial.get() : item.Material.get();
        const DrawableGeometry* geometry = item.Geometry.get();

        if (!geometry || !material)
            continue;

        DrawCallInfo info{};
        info.Geometry = geometry;
        info.Material = material;
        info.AABB = item.AABB;
        info.CastShadows = false;

        if (!settings.Filter(info))
            continue;

        if (EnableFrustumCulling && !frustum.IsVisible(item.AABB, settings.FrustumCullingPlanesBits))
            continue;

        m_TemporaryMatrices.push_back(item.Matrix);
        m_TemporaryMatrices.push_back(item.Matrix.Invert().Transpose());

        const GraphicsBackendBufferViewDescriptor viewDescriptor = GraphicsBackendBufferViewDescriptor::Structured(1, RenderQueueLocal::k_MatricesBufferElementSize, offset++ * RenderQueueLocal::k_MatricesBufferElementSize, false);
        std::shared_ptr<GraphicsBufferView> view = std::make_shared<GraphicsBufferView>(m_TemporaryMatricesBuffer, viewDescriptor, "RenderQueue/TemporaryMatricesSingleView");

        info.MatricesBufferViews.push_back(view);
        m_DrawCalls.push_back(info);
    }
}

void RenderQueue::BatchDrawCalls()
{
    Profiler::Marker _("RenderQueue::BatchDrawCalls");

    std::unordered_map<std::size_t, DrawCallInfo*> instancingMap;

    for (size_t i = 0; i < m_DrawCalls.size(); ++i)
    {
        DrawCallInfo& drawCall = m_DrawCalls[i];
        if (!drawCall.Material->GetShader()->SupportInstancing())
            continue;

        const size_t hash = RenderQueueLocal::GetDrawCallInstancingHash(drawCall);
        const auto it = instancingMap.find(hash);

        if (it == instancingMap.end())
        {
            drawCall.Instanced = true;
            instancingMap[hash] = &drawCall;
            continue;
        }

        DrawCallInfo* instancedDrawCall = it->second;
        instancedDrawCall->MatricesBufferViews.push_back(drawCall.MatricesBufferViews[0]);
        instancedDrawCall->AABB = instancedDrawCall->AABB.Combine(drawCall.AABB);

        m_DrawCalls[i] = m_DrawCalls[m_DrawCalls.size() - 1];
        m_DrawCalls.pop_back();
        --i;
    }

    uint32_t totalInstancesCount = 0;
    for (const DrawCallInfo& info : m_DrawCalls)
    {
        if (info.Instanced)
            totalInstancesCount += info.MatricesBufferViews.size();
    }

    const uint32_t requiredBufferSize = totalInstancesCount * sizeof(uint32_t);
    if (!m_InstancedMatricesEntriesBuffer || m_InstancedMatricesEntriesBuffer->GetSize() < requiredBufferSize)
    {
        GraphicsBackendBufferDescriptor descriptor{};
        descriptor.AllowCPUWrites = true;
        descriptor.Size = requiredBufferSize;

        m_InstancedMatricesEntriesBuffer = std::make_shared<GraphicsBuffer>(descriptor, "RenderQueue/InstancedMatricesEntries");
    }

    uint32_t instancesEntriesOffset = 0;
    for (DrawCallInfo& info : m_DrawCalls)
    {
        if (!info.Instanced)
            continue;

        const uint32_t instancesCount = info.MatricesBufferViews.size();

        m_InstancedMatricesEntries.reserve(instancesCount);
        m_InstancedMatricesEntriesCounts.push_back(instancesCount);
        for (const std::shared_ptr<GraphicsBufferView>& view : info.MatricesBufferViews)
            m_InstancedMatricesEntries.push_back(RenderQueueLocal::GetEntryFromBufferView(view));
        
        const GraphicsBackendBufferViewDescriptor descriptor = GraphicsBackendBufferViewDescriptor::Typed(TextureInternalFormat::R32F, instancesCount, instancesEntriesOffset * sizeof(uint32_t), false);
        info.InstancedMatricesEntriesView = std::make_shared<GraphicsBufferView>(m_InstancedMatricesEntriesBuffer, descriptor, "RenderQueue/InstancedMatricesEntriesView");

        instancesEntriesOffset += instancesCount;
    }
}

void RenderQueue::SetupMatrices(const DrawCallInfo& drawCallInfo) const
{
    const bool useTemporaryMatrices = !m_TemporaryMatrices.empty();

    if (drawCallInfo.Instanced)
    {
        GraphicsBackend::Current()->BindBuffer(drawCallInfo.InstancedMatricesEntriesView->GetBackendBufferView(), GlobalConstants::InstancingMatricesEntriesData);
        GraphicsBackend::Current()->BindBuffer(useTemporaryMatrices ? m_TemporaryMatricesBufferView->GetBackendBufferView() : s_PermanentMatricesBufferView->GetBackendBufferView(), GlobalConstants::TransformMatricesData);
    }
    else
	    GraphicsBackend::Current()->BindBuffer(drawCallInfo.MatricesBufferViews[0]->GetBackendBufferView(), GlobalConstants::TransformMatricesData);
}

void RenderQueue::SetupShaderPass(const Material* material, const VertexAttributes& vertexAttributes, PrimitiveType primitiveType, uint8_t stencilValue)
{
    if (m_PreviousMaterial != material)
    {
        uint32_t perMaterialDataBinding = 0;
        const std::shared_ptr<GraphicsBuffer>& perMaterialDataBuffer = material->GetPerMaterialDataBuffer(perMaterialDataBinding);
        if (perMaterialDataBuffer)
            GraphicsBackend::Current()->BindConstantBuffer(perMaterialDataBuffer->GetBackendBuffer(), perMaterialDataBinding, 0, perMaterialDataBuffer->GetSize());

        for (const auto& pair : material->GetTextures())
        {
            const uint32_t binding = pair.first;
            const std::shared_ptr<Texture>& texture = pair.second;

            if (!texture)
            {
                Debug::LogErrorFormat("[RenderQueue] Texture for binding {} is missing on material: {}", std::to_string(binding), material->GetName());
                continue;
            }

            GraphicsBackend::Current()->BindTextureSampler(texture->GetBackendTexture(), texture->GetBackendSampler(), binding);
        }

        GraphicsBackend::Current()->SetDepthState(material->DepthDescriptor);
        GraphicsBackend::Current()->SetRasterizerState(material->RasterizerDescriptor);
        GraphicsBackend::Current()->SetBlendState(material->BlendDescriptor);
        GraphicsBackend::Current()->SetStencilState(material->StencilDescriptor);
    }

    if (material->StencilDescriptor.Enabled)
        GraphicsBackend::Current()->SetStencilValue(stencilValue);

    if (m_PreviousMaterial != material || m_PreviousVertexAttributesHash != vertexAttributes.GetHash() || m_PreviousPrimitiveType != primitiveType)
		GraphicsBackend::Current()->UseProgram(material->GetShader()->GetProgram(vertexAttributes, primitiveType));

    m_PreviousMaterial = material;
    m_PreviousVertexAttributesHash = vertexAttributes.GetHash();
    m_PreviousPrimitiveType = primitiveType;
}

int RenderQueue::GetMatricesEntry()
{
    std::lock_guard<std::mutex> lock(s_FreeMatricesBufferEntriesMutex);

    if (s_FreeMatricesBufferEntries.empty())
	    return -1;

    const uint32_t entry = s_FreeMatricesBufferEntries.front();
    s_FreeMatricesBufferEntries.pop_front();
    return entry;
}

void RenderQueue::FreeMatricesEntry(const std::shared_ptr<GraphicsBufferView>& matricesBufferView)
{
    const int entry = RenderQueueLocal::GetEntryFromBufferView(matricesBufferView);
    if (entry < 0)
        return;

    std::lock_guard<std::mutex> lock(s_FreeMatricesBufferEntriesMutex);
    s_FreeMatricesBufferEntries.push_back(entry);
}

void RenderQueue::CheckMatricesBufferSize()
{
    Profiler::Marker _("RenderQueue::CheckMatricesBufferSize");

    {
        std::shared_lock lock(s_PermanentMatricesBufferRecreateMutex);
        if (!s_FreeMatricesBufferEntries.empty())
            return;
    }

    std::unique_lock lock(s_PermanentMatricesBufferRecreateMutex);

    if (s_FreeMatricesBufferEntries.empty())
    {
        s_MatricesBufferCapacity *= 2;
        CreatePermanentMatricesBuffer();
    }
}

void RenderQueue::CreatePermanentMatricesBuffer()
{
    GraphicsBackendBufferDescriptor bufferDescriptor{};
    bufferDescriptor.AllowCPUWrites = true;
    bufferDescriptor.Size = s_MatricesBufferCapacity * RenderQueueLocal::k_MatricesBufferElementSize;

    s_PermanentMatricesBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "RenderQueue/PermanentMatricesBuffer");
    s_FreeMatricesBufferEntries.resize(s_MatricesBufferCapacity);
    for (uint32_t i = 0; i < s_MatricesBufferCapacity; ++i)
        s_FreeMatricesBufferEntries[i] = i;

    GraphicsBackendBufferViewDescriptor viewDescriptor = GraphicsBackendBufferViewDescriptor::Structured(s_MatricesBufferCapacity, RenderQueueLocal::k_MatricesBufferElementSize, 0, false);
    s_PermanentMatricesBufferView = std::make_shared<GraphicsBufferView>(s_PermanentMatricesBuffer, viewDescriptor, "RenderQueue/PermanentMatricesBufferFullView");
}
