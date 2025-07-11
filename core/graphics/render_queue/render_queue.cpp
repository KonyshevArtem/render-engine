#include "render_queue.h"
#include "renderer/renderer.h"
#include "material/material.h"
#include "shader/shader.h"
#include "hash.h"
#include "global_constants.h"
#include "graphics/render_settings/render_settings.h"

std::size_t GetDrawCallInstancingHash(const DrawCallInfo& drawCallInfo)
{
    const std::size_t materialHash = std::hash<const Material*>{}(drawCallInfo.Material);
    const std::size_t geometryHash = std::hash<const DrawableGeometry*>{}(drawCallInfo.Geometry);
    return Hash::Combine(materialHash, geometryHash);
}

void SetupDrawCalls(const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& settings, std::vector<DrawCallInfo>& outDrawCalls)
{
    outDrawCalls.reserve(renderers.size());

    for (const std::shared_ptr<Renderer>& renderer : renderers)
    {
        if (renderer)
        {
            const DrawableGeometry* geometry = renderer->GetGeometry().get();
            const Material* material = settings.OverrideMaterial ? settings.OverrideMaterial.get() : renderer->GetMaterial().get();

            if (geometry && material)
            {
                DrawCallInfo info{
                    geometry,
                    material,
                    {renderer->GetModelMatrix()},
                    renderer->GetAABB(),
                    renderer->CastShadows,
                    false
                };
                outDrawCalls.push_back(info);
            }
        }
    }
}

void FilterDrawCalls(const DrawCallFilter& filter, std::vector<DrawCallInfo>& outDrawCalls)
{
    for (size_t i = 0; i < outDrawCalls.size(); ++i)
    {
        const DrawCallInfo& drawCall = outDrawCalls[i];
        if (!filter(drawCall))
        {
            outDrawCalls[i] = outDrawCalls[outDrawCalls.size() - 1];
            outDrawCalls.pop_back();
            --i;
        }
    }
}

void BatchDrawCalls(std::vector<DrawCallInfo>& outDrawCalls)
{
    std::unordered_map<std::size_t, DrawCallInfo*> instancingMap;

    for (size_t i = 0; i < outDrawCalls.size(); ++i)
    {
        DrawCallInfo& drawCall = outDrawCalls[i];
        if (!drawCall.Material->GetShader()->SupportInstancing())
        {
            continue;
        }

        const size_t hash = GetDrawCallInstancingHash(drawCall);
        const auto it = instancingMap.find(hash);

        if (it == instancingMap.end())
        {
            drawCall.Instanced = true;
            instancingMap[hash] = &drawCall;
            continue;
        }

        DrawCallInfo* instancedDrawCall = it->second;
        instancedDrawCall->ModelMatrices.push_back(drawCall.ModelMatrices[0]);
        instancedDrawCall->AABB = instancedDrawCall->AABB.Combine(drawCall.AABB);

        outDrawCalls[i] = outDrawCalls[outDrawCalls.size() - 1];
        outDrawCalls.pop_back();
        --i;

        if (instancedDrawCall->ModelMatrices.size() >= GlobalConstants::MaxInstancingCount)
            instancingMap.erase(it);
    }
}

void SortDrawCalls(DrawCallSortMode sortMode, const Vector3& cameraPosition, std::vector<DrawCallInfo>& outDrawCalls)
{
    if (sortMode != DrawCallSortMode::NO_SORTING)
        std::sort(outDrawCalls.begin(), outDrawCalls.end(), DrawCallComparer {sortMode, cameraPosition});
}

void RenderQueue::Prepare(const Vector3& cameraPosition, const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& renderSettings)
{
    m_DrawCalls.clear();

    SetupDrawCalls(renderers, renderSettings, m_DrawCalls);
    FilterDrawCalls(renderSettings.Filter, m_DrawCalls);
    BatchDrawCalls(m_DrawCalls);
    SortDrawCalls(renderSettings.Sorting, cameraPosition, m_DrawCalls);
}

void RenderQueue::Clear()
{
    m_DrawCalls.clear();
}

bool RenderQueue::IsEmpty() const
{
    return m_DrawCalls.empty();
}

const std::vector<DrawCallInfo>& RenderQueue::GetDrawCalls() const
{
    return m_DrawCalls;
}
