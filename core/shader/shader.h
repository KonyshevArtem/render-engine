#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "GLUT/glut.h"
#include "string"
#include "unordered_map"
#include "vector"

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

    SAMPLER_2D,
    SAMPLER_2D_ARRAY,
    SAMPLER_CUBE,
};

class Shader
{
public:
    static shared_ptr<Shader> Load(const string &_path, const vector<string> &_keywords, bool _silent = true);

    void SetUniform(const string &_name, const void *_data);

    ~Shader();

private:
    struct UniformInfo
    {
        UniformType Type;
        GLint       Location;
        int         Index;
    };

    explicit Shader(GLuint _program);

    GLuint                             m_Program;
    unordered_map<string, UniformInfo> m_Uniforms;

    inline static shared_ptr<Shader> FallbackShader = nullptr;
    static shared_ptr<Shader>        GetFallbackShader();

    static bool TryCompileShaderPart(GLuint                _shaderPartType,
                                     const string &        _path,
                                     const char *          _source,
                                     GLuint &              _shaderPart,
                                     const vector<string> &_keywords);

    static bool        TryLinkProgram(GLuint _vertexPart, GLuint _fragmentPart, GLuint &_program, const string &_path);
    static const char *GetShaderPartDefine(GLuint _shaderPartType);

    static UniformType ConvertUniformType(GLenum _type);

    friend class UniformBlock;
    friend class RenderPass;
    friend class ShadowCasterPass;
    friend class SkyboxPass;
};

#endif //OPENGL_STUDY_SHADER_H