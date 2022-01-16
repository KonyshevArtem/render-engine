#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "shader_loader/shader_loader.h"
#include <OpenGL/gl3.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Texture;
struct UniformInfo;

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

    ~Shader();

private:
    Shader(GLuint                        _program,
           unordered_map<string, string> _defaultValues,
           unordered_map<string, string> _tags,
           bool                          _zWrite,
           BlendInfo                     _blendInfo);
    Shader(const Shader &) = delete;
    Shader(Shader &&)      = delete;

    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&)      = delete;

#pragma endregion

#pragma region fields

private:
    GLuint                             m_Program;
    bool                               m_ZWrite;
    BlendInfo                          m_BlendInfo;
    unordered_map<string, UniformInfo> m_Uniforms;
    unordered_map<string, int>         m_TextureUnits;
    unordered_map<string, string>      m_DefaultValues;
    unordered_map<string, string>      m_Tags;

    static unordered_map<string, shared_ptr<Texture>> m_GlobalTextures;
    static string                                     m_ReplacementTag;
    static const Shader *                             m_CurrentShader;
    static const Shader *                             m_ReplacementShader;

#pragma endregion

#pragma region public methods

public:
    bool Use() const;

    static void SetUniform(const string &_name, const void *_data);
    static void SetTextureUniform(const string &_name, const Texture &_texture);
    static void SetReplacementShader(const Shader *_shader, const string &_tag);
    static void DetachReplacementShader();
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