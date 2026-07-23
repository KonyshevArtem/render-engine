#ifndef RENDER_ENGINE_RAYTRACING_PROBES_H
#define RENDER_ENGINE_RAYTRACING_PROBES_H

#include "file_watcher.h"
#include "vector2/vector2.h"
#include "vector3/vector3.h"
#include "graphics/passes/render_pass.h"
#include "texture/texture_resources.h"

struct RenderData;
class Shader;
class RaytracingScene;
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

        Vector3I GridCenter;
        float Padding0;

        Vector3I GridOffset;
        float Padding1;
	};

    bool m_RaytracingProbesEnabled;
    bool m_DebugDrawProbes;
    bool m_DebugDrawProbeGI;

	float m_ProbeSpacing;
	Vector3I m_ProbesGridSize;
    Vector3I m_ProbesGridOffset;
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
    TextureResources m_ProbeTempLightAtlas;
    TextureResources m_ProbeTempDepthAtlas;
    TextureResources m_ProbeLightAtlas;
    TextureResources m_ProbeDepthAtlas;
    TextureResources m_DebugProbeGITarget;

    FileWatcher m_FileWatcher;

    void LoadShaders(bool reload);
	void UpdateProbesData() const;
	void UpdateTextureResources(TextureResources& textureResources, uint32_t width, uint32_t height, TextureInternalFormat format, const std::string& name) const;
    void ClearTextureResources();
};

#endif