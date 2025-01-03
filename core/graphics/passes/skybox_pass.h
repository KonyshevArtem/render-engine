#ifndef RENDER_ENGINE_SKYBOX_PASS_H
#define RENDER_ENGINE_SKYBOX_PASS_H

#include "render_pass.h"

struct Context;
class Mesh;

class SkyboxPass : public RenderPass
{
public:
    explicit SkyboxPass(int priority);
    ~SkyboxPass() override = default;

    void Prepare();
    void Execute(const Context& ctx) override;

    SkyboxPass(const SkyboxPass&) = delete;
    SkyboxPass(SkyboxPass&&) = delete;

    SkyboxPass &operator=(const SkyboxPass&) = delete;
    SkyboxPass &operator=(SkyboxPass&&) = delete;

private:
    static std::shared_ptr<Mesh> m_Mesh;
};

#endif //RENDER_ENGINE_SKYBOX_PASS_H
