#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include "../../core/material/material.h"
#include "../../core/shader/shader.h"
#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/vector3/vector3.h"
#include "OpenGL/gl3.h"

class Graphics
{
public:
    static void          Init(int _argc, char **_argv);
    static void          Render();
    static void          Reshape(int _width, int _height);
    static const string &GetShaderCompilationDefines();

    inline static int ScreenWidth  = 0;
    inline static int ScreenHeight = 0;

private:
    static const int MAX_LIGHT_SOURCES = 3;
    static const int GLSL_VERSION      = 410;

    inline static unique_ptr<UniformBlock> LightingDataBlock = nullptr;
    inline static unique_ptr<UniformBlock> CameraDataBlock   = nullptr;
    inline static string                   ShaderCompilationDefine;

    static void InitCulling();
    static void InitDepth();
    static void InitFramebuffer();
    static void InitUniformBlocks();

    static void BindDefaultTextures(const shared_ptr<Shader> &_shader, int &_textureUnits);
    static void TransferUniformsFromMaterial(const shared_ptr<Material> &_material);
    static void UpdateCameraData(Vector3 _cameraPosWS, Matrix4x4 _viewMatrix, Matrix4x4 _projectionMatrix);
    static void UpdateLightingData();
};

#endif //OPENGL_STUDY_GRAPHICS_H
