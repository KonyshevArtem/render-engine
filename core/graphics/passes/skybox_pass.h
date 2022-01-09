#ifndef OPENGL_STUDY_SKYBOX_PASS_H
#define OPENGL_STUDY_SKYBOX_PASS_H

class Context;

class SkyboxPass
{
public:
    SkyboxPass() = default;

    void Execute(const Context &_ctx);

private:
    SkyboxPass(const SkyboxPass &) = delete;
};

#endif //OPENGL_STUDY_SKYBOX_PASS_H
