#ifndef RENDER_ENGINE_RAYTRACING_PROBES_H
#define RENDER_ENGINE_RAYTRACING_PROBES_H

#include "file_watcher.h"
#include "vector2/vector2.h"
#include "vector3/vector3.h"
#include "graphics/passes/render_pass.h"

struct RenderData;
class Shader;
class RaytracingScene;
class Texture;
class GraphicsBuffer;
class Mesh;

class RaytracingProbes : public RenderPass
{
public:
    RaytracingProbes(const std::shared_ptr<RaytracingScene>& rtScene);
    ~RaytracingProbes() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;
	void ExecuteDebug(const RenderData& renderData);

	void BindResources() const;

private:
    struct ProbesData
    {
        Vector3I GridSize;
        float Spacing;

        uint32_t ProbesPerRow;
        uint32_t ProbeCount;
        uint32_t UpdateProbeBaseIndex;
        uint32_t ProbeLightSize;

        uint32_t ProbesUpdatePerFrame;
        uint32_t ProbeLightPaddedSize;
		Vector2 InvProbeAtlasSize;
	};

    bool m_RaytracingProbesEnabled;
    bool m_DebugDrawProbes;
    bool m_DebugDrawProbeGI;

	float m_ProbeSpacing;
	Vector3I m_ProbesGridSize;
    uint32_t m_ProbesPerUpdate;
	uint32_t m_UpdateProbeBaseIndex;
    uint32_t m_ProbeLightSize;
    uint32_t m_ProbeLightPadding;

    std::shared_ptr<Shader> m_ProbeTraceShader;
    std::shared_ptr<Shader> m_ProbeIntegrateShader;
    std::shared_ptr<Shader> m_ProbeAddBorderShader;
    std::shared_ptr<Shader> m_DebugDrawProbesShader;
    std::shared_ptr<Shader> m_DebugDrawProbeGIShader;

    std::shared_ptr<Mesh> m_SphereMesh;
    std::shared_ptr<RaytracingScene> m_RaytracingScene;
	std::shared_ptr<GraphicsBuffer> m_ProbesDataBuffer;
	std::shared_ptr<GraphicsBuffer> m_ProbesDebugDataBuffer;
    std::shared_ptr<Texture> m_ProbeTempLightAtlas;
    std::shared_ptr<Texture> m_ProbeTempDepthAtlas;
    std::shared_ptr<Texture> m_ProbeLightAtlas;
    std::shared_ptr<Texture> m_ProbeDepthAtlas;
    std::shared_ptr<Texture> m_DebugProbeGITarget;

    FileWatcher m_FileWatcher;

    void LoadShaders(bool reload);
	void UpdateProbesData() const;
};

#endif