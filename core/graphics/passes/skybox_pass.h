#ifndef RENDER_ENGINE_SKYBOX_PASS_H
#define RENDER_ENGINE_SKYBOX_PASS_H

#include "render_pass.h"

struct RenderData;
class Mesh;

class SkyboxPass : public RenderPass
{
public:
    SkyboxPass() = default;
    ~SkyboxPass() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;

    SkyboxPass(const SkyboxPass&) = delete;
    SkyboxPass(SkyboxPass&&) = delete;

    SkyboxPass &operator=(const SkyboxPass&) = delete;
    SkyboxPass &operator=(SkyboxPass&&) = delete;

private:
    static std::shared_ptr<Mesh> m_Mesh;
};

#endif //RENDER_ENGINE_SKYBOX_PASS_H
