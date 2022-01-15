#ifndef OPENGL_STUDY_SKYBOX_PASS_H
#define OPENGL_STUDY_SKYBOX_PASS_H

class Context;

class SkyboxPass
{
public:
    SkyboxPass()  = default;
    ~SkyboxPass() = default;

    void Execute(const Context &_ctx);

private:
    SkyboxPass(const SkyboxPass &) = delete;
    SkyboxPass(SkyboxPass &&)      = delete;

    SkyboxPass &operator()(const SkyboxPass &) = delete;
    SkyboxPass &operator()(SkyboxPass &&)      = delete;
};

#endif //OPENGL_STUDY_SKYBOX_PASS_H
