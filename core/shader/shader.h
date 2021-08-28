#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/vector4/vector4.h"
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
};

class Shader
{
public:
    GLuint Program;

    static shared_ptr<Shader> Load(const string &_path);

    void SetUniform(const string &_name, const void *_data);

    ~Shader();

private:
    explicit Shader(GLuint _program);

    unordered_map<string, UniformInfo> m_Uniforms;

    static GLuint CompileShaderPart(GLuint _shaderPartType, const string &_path);
    static GLuint LinkProgram(GLuint _vertexPart, GLuint _fragmentPart);

    static UniformType ConvertUniformType(GLenum _type);
};

#endif //OPENGL_STUDY_SHADER_H