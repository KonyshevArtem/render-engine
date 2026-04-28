#ifndef RENDER_ENGINE_RAYTRACING_PASS_H
#define RENDER_ENGINE_RAYTRACING_PASS_H

#include "file_system/file_watcher.h"
#include "graphics/passes/render_pass.h"

#include <random>

struct RenderData;
class Shader;
class RaytracingScene;
class Texture;
class GraphicsBuffer;

class RaytracingPass : public RenderPass
{
public:
    RaytracingPass(const std::shared_ptr<RaytracingScene>& rtScene);
    ~RaytracingPass() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;

    void ExecuteRaytracedShadows(const RenderData& renderData);
    void ExecutePrimaryRaysDebug(const RenderData& renderData);

private:
    std::shared_ptr<Shader> m_PrimaryRaysDebugShader;
    std::shared_ptr<GraphicsBuffer> m_PrimaryRaysDebugDataBuffer;
    bool m_PrimaryRaysDebugEnabled;

    std::shared_ptr<Shader> m_RaytracedShadowsShader;
    std::shared_ptr<Texture> m_RaytracedShadowsTarget;
    std::shared_ptr<GraphicsBuffer> m_RaytracedShadowsDataBuffer;
    bool m_RaytracedShadowsEnabled;
    int m_RaytracedShadowsSamplesCount = 1;

    std::shared_ptr<RaytracingScene> m_RaytracingScene;
    std::shared_ptr<Texture> m_BlueNoiseTexture;

	FileWatcher m_FileWatcher;
    std::mt19937 m_Rng;

    void LoadShaders();
};


#endif //RENDER_ENGINE_RAYTRACING_PASS_H
