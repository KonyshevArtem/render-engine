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
class Texture2DArray;
class Texture2D;
class Renderer;

class ShadowCasterPass
{
public:
    explicit ShadowCasterPass(std::shared_ptr<UniformBlock> shadowsUniformBlock);
    ~ShadowCasterPass() = default;

    void Execute(const Context &_ctx);

    ShadowCasterPass(const ShadowCasterPass &) = delete;
    ShadowCasterPass(ShadowCasterPass &&)      = delete;

    ShadowCasterPass &operator=(const ShadowCasterPass &) = delete;
    ShadowCasterPass &operator=(ShadowCasterPass &&) = delete;

private:
    static constexpr int SPOT_LIGHT_SHADOW_MAP_SIZE = 1024;
    static constexpr int DIR_LIGHT_SHADOW_MAP_SIZE  = 2048;
    static constexpr int POINT_LIGHT_SHADOW_MAP_FACE_SIZE = 512;

    std::shared_ptr<UniformBlock>   m_ShadowsUniformBlock;
    std::shared_ptr<Texture2DArray> m_SpotLightShadowMapArray;
    std::shared_ptr<Texture2D>      m_DirectionLightShadowMap;
    std::shared_ptr<Texture2DArray> m_PointLightShadowMap;

    static void Render(const std::vector<Renderer *> &renderers);
};

#endif //OPENGL_STUDY_SHADOW_CASTER_PASS_H
