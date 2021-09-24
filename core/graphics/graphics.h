#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include "OpenGL/gl3.h"
#include "string"

struct Matrix4x4;
struct Vector4;
class RenderPass;
class ShadowCasterPass;
class UniformBlock;
class Light;

using namespace std;

class Graphics
{
public:
    static void          Init(int _argc, char **_argv);
    static void          Render();
    static void          Reshape(int _width, int _height);
    static const string &GetShaderCompilationDefines();

    static void SetCameraData(Matrix4x4 _viewMatrix, Matrix4x4 _projectionMatrix);

    inline static int ScreenWidth  = 0;
    inline static int ScreenHeight = 0;

private:
    static const inline int MAX_POINT_LIGHT_SOURCES = 3;
    static const inline int MAX_SPOT_LIGHT_SOURCES  = 3;
    static const inline int GLSL_VERSION            = 410;

    static unique_ptr<UniformBlock> LightingDataBlock;
    static unique_ptr<UniformBlock> CameraDataBlock;
    static shared_ptr<UniformBlock> ShadowsDataBlock;
    inline static string            ShaderCompilationDefine;

    static unique_ptr<ShadowCasterPass> m_ShadowCasterPass;
    static unique_ptr<RenderPass>       m_RenderPass;

    static void InitCulling();
    static void InitDepth();
    static void InitFramebuffer();
    static void InitUniformBlocks();
    static void InitPasses();

    static void SetLightingData(Vector4 _ambient, const vector<shared_ptr<Light>> &_lights);
};

#endif //OPENGL_STUDY_GRAPHICS_H
