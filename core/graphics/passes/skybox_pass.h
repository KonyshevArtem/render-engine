#ifndef RENDER_ENGINE_SKYBOX_PASS_H
#define RENDER_ENGINE_SKYBOX_PASS_H

struct Context;

class SkyboxPass
{
public:
    SkyboxPass()  = default;
    ~SkyboxPass() = default;

    static void Execute(const Context &_ctx);

    SkyboxPass(const SkyboxPass &) = delete;
    SkyboxPass(SkyboxPass &&)      = delete;

    SkyboxPass &operator=(const SkyboxPass &) = delete;
    SkyboxPass &operator=(SkyboxPass &&)      = delete;
};

#endif //RENDER_ENGINE_SKYBOX_PASS_H
