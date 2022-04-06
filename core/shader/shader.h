#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#include "matrix4x4/matrix4x4.h"
#include "property_block/property_block.h"
#include "shader_loader/shader_loader.h"
#include "uniform_info/uniform_info.h"
#include "vector4/vector4.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <filesystem>
#include <string>
#include <unordered_map>


class Texture;

class Shader
{
#pragma region inner types

public:
    struct BlendInfo
    {
        bool   Enabled = false;
        GLenum SrcFactor;
        GLenum DstFactor;
    };

    struct CullInfo
    {
        bool   Enabled = true;
        GLenum Face    = GL_BACK;
    };

    struct DepthInfo
    {
        bool   ZWrite = true;
        GLenum ZTest  = GL_LEQUAL;
    };

    struct PassInfo
    {
        GLuint                                       Program;
        BlendInfo                                    BlendInfo;
        CullInfo                                     CullInfo;
        DepthInfo                                    DepthInfo;
        std::unordered_map<std::string, std::string> Tags;
        std::unordered_map<std::string, UniformInfo> Uniforms;
        std::unordered_map<std::string, int>         TextureUnits;
    };

#pragma endregion

#pragma region construction

public:
    static std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);

    ~Shader();

private:
    Shader(std::vector<PassInfo> _passes, std::unordered_map<std::string, std::string> _defaultValues);
    Shader(const Shader &) = delete;
    Shader(Shader &&)      = delete;

    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

#pragma endregion

#pragma region fields

private:
    std::vector<PassInfo>                        m_Passes;
    std::unordered_map<std::string, std::string> m_DefaultValues;

    static PropertyBlock   m_PropertyBlock;
    static const PassInfo *m_CurrentPass;

#pragma endregion

#pragma region public methods

public:
    void        Use(int _passIndex) const;
    std::string GetPassTagValue(int _passIndex, const std::string &_tag) const;

    inline int PassesCount() const
    {
        return m_Passes.size();
    }

    static void SetPropertyBlock(const PropertyBlock &_propertyBlock);

    static void SetGlobalTexture(const std::string &_name, std::shared_ptr<Texture> _value);
    static void SetGlobalVector(const std::string &_name, const Vector4 &_value);
    static void SetGlobalFloat(const std::string &_name, float _value);
    static void SetGlobalMatrix(const std::string &_name, const Matrix4x4 &_value);

    static const std::shared_ptr<Texture> GetGlobalTexture(const std::string &_name);
    static Vector4                        GetGlobalVector(const std::string &_name);
    static float                          GetGlobalFloat(const std::string &_name);
    static Matrix4x4                      GetGlobalMatrix(const std::string &_name);

#pragma endregion

#pragma region service methods

private:
    void        SetBlendInfo(const BlendInfo &_blendInfo) const;
    void        SetCullInfo(const CullInfo &_cullInfo) const;
    void        SetDepthInfo(const DepthInfo &_depthInfo) const;
    void        SetDefaultValues(const std::unordered_map<std::string, UniformInfo> &_uniforms) const;
    static void SetUniform(const std::string &_name, const void *_data);
    static void SetTextureUniform(const std::string &_name, const Texture &_texture);

#pragma endregion

    friend std::shared_ptr<Shader> ShaderLoader::Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);

    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H