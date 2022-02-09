#ifndef OPENGL_STUDY_SHADOW_CASTER_PASS_H
#define OPENGL_STUDY_SHADOW_CASTER_PASS_H

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <memory>
#include <vector>

class UniformBlock;
struct Context;
class Shader;
class Texture2DArray;
class Texture2D;
class Renderer;

class ShadowCasterPass
{
public:
    ShadowCasterPass(int _spotLightsCount, std::shared_ptr<UniformBlock> _shadowsUniformBlock);
    ~ShadowCasterPass();

    void Execute(const Context &_ctx);

private:
    ShadowCasterPass(const ShadowCasterPass &) = delete;
    ShadowCasterPass(ShadowCasterPass &&)      = delete;

    ShadowCasterPass &operator=(const ShadowCasterPass &) = delete;
    ShadowCasterPass &operator=(ShadowCasterPass &&)      = delete;

    static constexpr int SHADOW_MAP_SIZE = 1024;

    std::shared_ptr<UniformBlock>   m_ShadowsUniformBlock;
    std::shared_ptr<Shader>         m_ShadowCasterShader;
    GLuint                          m_Framebuffer = 0;
    std::shared_ptr<Texture2DArray> m_SpotLightShadowMapArray;
    std::shared_ptr<Texture2D>      m_DirectionLightShadowMap;

    void Render(const std::vector<Renderer *> &_gameObjects);
};

#endif //OPENGL_STUDY_SHADOW_CASTER_PASS_H
