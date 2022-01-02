#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "shader_loader/shader_loader.h"
#include "uniform_type/uniform_type.h"
#include <OpenGL/gl3.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Texture;
class BaseUniform;

class Shader
{
#pragma region inner types

public:
    struct BlendInfo
    {
        bool   Enabled = false;
        GLenum SrcFactor;
        GLenum DstFactor;

        void Apply() const;
    };

#pragma endregion

#pragma region construction

public:
    static shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords);

    virtual ~Shader();

private:
    Shader(GLuint                        _program,
           unordered_map<string, string> _defaultValues,
           bool                          _zWrite,
           BlendInfo                     _blendInfo);
    Shader(const Shader &) = delete;

#pragma endregion

#pragma region fields

private:
    GLuint                                            m_Program;
    bool                                              m_ZWrite;
    BlendInfo                                         m_BlendInfo;
    unordered_map<string, shared_ptr<BaseUniform>>    m_Uniforms;
    unordered_map<string, int>                        m_TextureUnits;
    unordered_map<string, string>                     m_DefaultValues;
    static unordered_map<string, shared_ptr<Texture>> m_GlobalTextures;
    static const Shader *                             m_CurrentShader;

#pragma endregion

#pragma region public methods

public:
    void Use() const;
    void SetUniform(const string &_name, const void *_data) const;
    void SetTextureUniform(const string &_name, const Texture &_texture) const;

    static void DetachCurrentShader();
    static void SetGlobalTexture(const string &_name, shared_ptr<Texture> _texture);

#pragma endregion

#pragma region service methods

private:
    void SetDefaultValues() const;

#pragma endregion

    friend shared_ptr<Shader> ShaderLoader::Load(const filesystem::path &_path, const vector<string> &_keywords);
    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H