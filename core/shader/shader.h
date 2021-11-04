#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "uniform_type.h"
#include <GLUT/glut.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Texture;

class Shader
{
public:
    //region construction

    static shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords, bool _silent = true);
    ~Shader();

    //endregion

    //region instance methods

    void Use() const;
    void SetUniform(const string &_name, const void *_data) const;
    void SetTextureUniform(const string &_name, const shared_ptr<Texture> &_texture) const;

    //endregion

    //region static methods

    static void                      DetachCurrentShader();
    static void                      SetGlobalTexture(const string &_name, const shared_ptr<Texture> &_texture);
    static const shared_ptr<Shader> &GetFallbackShader();

    //endregion

private:
    //region inner types

    struct UniformInfo
    {
        UniformType Type;
        GLint       Location;
        int         Index;
    };

    static UniformType ConvertUniformType(GLenum _type);

    //endregion

    //region construction

    explicit Shader(GLuint _program);

    static bool TryCompileShaderPart(GLuint                _shaderPartType,
                                     const string         &_path,
                                     const char           *_source,
                                     GLuint               &_shaderPart,
                                     const vector<string> &_keywords);

    static bool        TryLinkProgram(GLuint _vertexPart, GLuint _fragmentPart, GLuint &_program, const string &_path);
    static const char *GetShaderPartDefine(GLuint _shaderPartType);

    //endregion

    //region fields

    GLuint                                            m_Program;
    unordered_map<string, UniformInfo>                m_Uniforms;
    unordered_map<string, int>                        m_TextureUnits;
    static unordered_map<string, shared_ptr<Texture>> m_GlobalTextures;
    inline static shared_ptr<Shader>                  m_FallbackShader = nullptr;
    inline static const Shader                       *m_CurrentShader  = nullptr;

    //endregion

    //region instance methods

    void BindDefaultTextures() const;

    //endregion

    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H