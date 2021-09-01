#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/vector4/vector4.h"
#include "../graphics/uniform_block.h"
#include "GLUT/glut.h"
#include "string"
#include "unordered_map"

using namespace std;

enum UniformType
{
    UNKNOWN,

    INT,

    FLOAT,
    FLOAT_VEC3,
    FLOAT_VEC4,
    FLOAT_MAT4,

    BOOL,

    SAMPLER_2D
};

struct UniformInfo
{
    UniformType Type;
    GLint       Location;
    int         Index;
};

class Shader
{
public:
    static shared_ptr<Shader> Load(const string &_path, bool _silent = true);

    void SetUniform(const string &_name, const void *_data);

    ~Shader();

private:
    explicit Shader(GLuint _program);

    GLuint                             m_Program;
    unordered_map<string, UniformInfo> m_Uniforms;

    inline static shared_ptr<Shader> FallbackShader = nullptr;
    static shared_ptr<Shader>        GetFallbackShader();

    static bool        TryCompileShaderPart(GLuint _shaderPartType, const string &_path, const char *_source, GLuint &_shaderPart);
    static bool        TryLinkProgram(GLuint _vertexPart, GLuint _fragmentPart, GLuint &_program);
    static const char *GetShaderPartDefine(GLuint _shaderPartType);

    static UniformType ConvertUniformType(GLenum _type);

    static shared_ptr<Shader> LoadForInit(const string &_path);

    friend class Graphics;
    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H