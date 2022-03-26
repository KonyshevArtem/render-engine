#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#include "shader_loader/shader_loader.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <filesystem>
#include <string>
#include <unordered_map>

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
    static std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);

    ~Shader();

private:
    Shader(GLuint                                       _program,
           std::unordered_map<std::string, std::string> _defaultValues,
           std::unordered_map<std::string, std::string> _tags,
           bool                                         _zWrite,
           GLenum                                       _zTest,
           BlendInfo                                    _blendInfo);
    Shader(const Shader &) = delete;
    Shader(Shader &&)      = delete;

    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

#pragma endregion

#pragma region fields

private:
    GLuint                                       m_Program;
    bool                                         m_ZWrite;
    BlendInfo                                    m_BlendInfo;
    GLenum                                       m_ZTest;
    std::unordered_map<std::string, UniformInfo> m_Uniforms;
    std::unordered_map<std::string, int>         m_TextureUnits;
    std::unordered_map<std::string, std::string> m_DefaultValues;
    std::unordered_map<std::string, std::string> m_Tags;

    static std::unordered_map<std::string, std::shared_ptr<Texture>> m_GlobalTextures;
    static std::string                                               m_ReplacementTag;
    static const Shader                                             *m_CurrentShader;
    static const Shader                                             *m_ReplacementShader;

#pragma endregion

#pragma region public methods

public:
    bool Use() const;

    static void SetUniform(const std::string &_name, const void *_data);
    static void SetTextureUniform(const std::string &_name, const Texture &_texture);
    static void SetReplacementShader(const Shader *_shader, const std::string &_tag);
    static void DetachReplacementShader();
    static void DetachCurrentShader();
    static void SetGlobalTexture(const std::string &_name, std::shared_ptr<Texture> _texture);

#pragma endregion

#pragma region service methods

private:
    void SetDefaultValues() const;

#pragma endregion

    friend std::shared_ptr<Shader> ShaderLoader::Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);
    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H