#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include <string>

struct Matrix4x4;
struct Vector4;
class RenderPass;
class ShadowCasterPass;
class SkyboxPass;
class UniformBlock;
class Light;

using namespace std;

class Graphics
{
public:
    static void          Init(int _argc, char **_argv);
    static void          Render();
    static void          Reshape(int _width, int _height);
    static const string &GetGlobalShaderDirectives();
    static void          SetCameraData(const Matrix4x4 &_viewMatrix, const Matrix4x4 &_projectionMatrix);

    inline static int GetScreenWidth()
    {
        return m_ScreenWidth;
    }

    inline static int GetScreenHeight()
    {
        return m_ScreenHeight;
    }

private:
    Graphics()                 = delete;
    Graphics(const Graphics &) = delete;

    static constexpr int MAX_POINT_LIGHT_SOURCES = 3;
    static constexpr int MAX_SPOT_LIGHT_SOURCES  = 3;
    static constexpr int GLSL_VERSION            = 410;

    static unique_ptr<UniformBlock> LightingDataBlock;
    static unique_ptr<UniformBlock> CameraDataBlock;
    static shared_ptr<UniformBlock> ShadowsDataBlock;

    static unique_ptr<ShadowCasterPass> m_ShadowCasterPass;
    static unique_ptr<RenderPass>       m_RenderPass;
    static unique_ptr<SkyboxPass>       m_SkyboxPass;

    static string m_GlobalShaderDirectives;
    static int    m_ScreenWidth;
    static int    m_ScreenHeight;

    static void InitCulling();
    static void InitDepth();
    static void InitFramebuffer();
    static void InitUniformBlocks();
    static void InitPasses();

    static void SetLightingData(const Vector4 &_ambient, const vector<shared_ptr<Light>> &_lights);
};

#endif //OPENGL_STUDY_GRAPHICS_H
