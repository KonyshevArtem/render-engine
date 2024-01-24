#ifndef OPENGL_STUDY_SKYBOX_PASS_H
#define OPENGL_STUDY_SKYBOX_PASS_H

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

#endif //OPENGL_STUDY_SKYBOX_PASS_H
