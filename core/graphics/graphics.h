#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/vector3/vector3.h"
#include "../light/light.h"
#include "../material/material.h"
#include "../shader/shader.h"
#include "OpenGL/gl3.h"

class Graphics
{
public:
    static void          Init(int _argc, char **_argv);
    static void          Render();
    static void          Reshape(int _width, int _height);
    static const string &GetShaderCompilationDefines();

    ~Graphics();

    inline static int ScreenWidth  = 0;
    inline static int ScreenHeight = 0;

private:
    static const int MAX_POINT_LIGHT_SOURCES = 3;
    static const int MAX_SPOT_LIGHT_SOURCES  = 3;
    static const int GLSL_VERSION            = 410;

    inline static unique_ptr<UniformBlock> LightingDataBlock = nullptr;
    inline static unique_ptr<UniformBlock> CameraDataBlock   = nullptr;
    inline static string                   ShaderCompilationDefine;

    inline static shared_ptr<Texture> ShadowMap          = nullptr;
    inline static shared_ptr<Shader>  ShadowCasterShader = nullptr;
    inline static GLuint              ShadowFramebuffer  = 0;

    static void InitCulling();
    static void InitDepth();
    static void InitFramebuffer();
    static void InitUniformBlocks();
    static void InitShadows();

    static void ShadowCasterPass();
    static void RenderPass();

    static void BindDefaultTextures(const shared_ptr<Shader> &_shader, unordered_map<string, int> &_textureUnits);
    static void TransferUniformsFromMaterial(const shared_ptr<Material> &_material);
    static void UpdateCameraData(Vector3 _cameraPosWS, Matrix4x4 _viewMatrix, Matrix4x4 _projectionMatrix);
    static void UpdateLightingData(Vector4 _ambient, const vector<shared_ptr<Light>> &_lights);
};

#endif //OPENGL_STUDY_GRAPHICS_H
