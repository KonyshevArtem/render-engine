#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include <GLUT/glut.h>
#include <filesystem>
#include <string>
#include "uniform_type.h"
#include <unordered_map>
#include <vector>

using namespace std;

class Shader
{
public:
    static shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords, bool _silent = true);

    void                                            Use() const;
    [[nodiscard]] const unordered_map<string, int> &GetTextureUnits() const;
    void                                            SetUniform(const string &_name, const void *_data) const;
    void                                            BindDefaultTextures() const;

    static const shared_ptr<Shader> &GetFallbackShader();

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
    unordered_map<string, int>         m_TextureUnits;

    inline static shared_ptr<Shader> FallbackShader = nullptr;

    static bool TryCompileShaderPart(GLuint                _shaderPartType,
                                     const string         &_path,
                                     const char           *_source,
                                     GLuint               &_shaderPart,
                                     const vector<string> &_keywords);

    static bool        TryLinkProgram(GLuint _vertexPart, GLuint _fragmentPart, GLuint &_program, const string &_path);
    static const char *GetShaderPartDefine(GLuint _shaderPartType);

    static UniformType ConvertUniformType(GLenum _type);

    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H