#ifndef RENDER_ENGINE_RAYTRACING_PASS_H
#define RENDER_ENGINE_RAYTRACING_PASS_H

#include "graphics/passes/render_pass.h"

struct RenderData;
class Shader;
class RaytracingScene;
class Texture2D;
class GraphicsBuffer;

class RaytracingPass : public RenderPass
{
public:
    RaytracingPass(const std::shared_ptr<RaytracingScene>& rtScene);
    ~RaytracingPass() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;

    void ExecutePrimaryRaysDebug(const RenderData& renderData);

private:
    std::shared_ptr<Shader> m_PrimaryRaysDebugShader;
    std::shared_ptr<GraphicsBuffer> m_PrimaryRaysDebugDataBuffer;
    bool m_PrimaryRaysDebugEnabled;

    std::shared_ptr<RaytracingScene> m_RaytracingScene;
};


#endif //RENDER_ENGINE_RAYTRACING_PASS_H
