#ifndef OPENGL_STUDY_SHADOW_CASTER_PASS_H
#define OPENGL_STUDY_SHADOW_CASTER_PASS_H

#include <OpenGL/gl3.h>
#include <memory>
#include <vector>

class UniformBlock;
class Context;
class Shader;
class Texture2DArray;
class GameObject;

using namespace std;

class ShadowCasterPass
{
public:
    ShadowCasterPass(int _spotLightsCount, shared_ptr<UniformBlock> _shadowsUniformBlock);
    ~ShadowCasterPass();

    void Execute(const shared_ptr<Context> &_ctx);

private:
    ShadowCasterPass(const ShadowCasterPass &) = delete;

    static constexpr int SHADOW_MAP_SIZE = 1024;

    shared_ptr<UniformBlock>   m_ShadowsUniformBlock;
    shared_ptr<Shader>         m_ShadowCasterShader;
    GLuint                     m_Framebuffer = 0;
    shared_ptr<Texture2DArray> m_SpotLightShadowMapArray;

    void Render(const vector<shared_ptr<GameObject>> &_gameObjects);
};

#endif //OPENGL_STUDY_SHADOW_CASTER_PASS_H
