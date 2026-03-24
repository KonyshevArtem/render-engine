#ifndef RENDER_ENGINE_GIZMOS_PASS_H
#define RENDER_ENGINE_GIZMOS_PASS_H

#if RENDER_ENGINE_EDITOR

#include "graphics/passes/render_pass.h"
#include "graphics/render_queue/render_queue.h"

struct RenderData;
class Renderer;
class Texture;

class GizmosPass : public RenderPass
{
public:
    enum class Mode
    {
	    GIZMOS_2D,
        GIZMOS_3D
    };

    GizmosPass(int priority, Mode mode);
    ~GizmosPass() override = default;

    bool Prepare(const RenderData& renderData);
    void Execute(const RenderData& renderData) override;

    GizmosPass(const GizmosPass&) = delete;
    GizmosPass(GizmosPass&&) = delete;

    GizmosPass &operator=(const GizmosPass&) = delete;
    GizmosPass &operator=(GizmosPass&&) = delete;

private:
    RenderQueue m_GizmosQueue;
    Matrix4x4 m_ViewMatrix;
    Matrix4x4 m_ProjectionMatrix;
    float m_NearPlane;
    float m_FarPlane;
    Mode m_Mode;

    std::shared_ptr<Texture> m_ColorTarget;
    std::shared_ptr<Texture> m_DepthTarget;
};

#endif
#endif