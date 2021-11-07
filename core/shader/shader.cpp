#include "shader.h"
#include "../../math/vector4/vector4.h"
#include "../../utils/utils.h"
#include "../texture_2d/texture_2d.h"
#include "shader_loader/shader_loader.h"
#include "uniform/base_uniform.h"
#include <OpenGL/gl3.h>

using namespace std;

//region construction

shared_ptr<Shader> Shader::Load(const filesystem::path &_path, const vector<string> &_keywords, bool _silent)
{
    return ShaderLoader::Load(_path, _keywords, _silent);
}

Shader::Shader(GLuint _program, unordered_map<string, string> _defaultValues) :
    m_Program(_program), m_DefaultValues(std::move(_defaultValues))
{
    GLuint lightingUniformIndex   = glGetUniformBlockIndex(m_Program, "Lighting");
    GLuint cameraDataUniformIndex = glGetUniformBlockIndex(m_Program, "CameraData");
    GLuint shadowDataUniformIndex = glGetUniformBlockIndex(m_Program, "Shadows");

    if (cameraDataUniformIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(m_Program, cameraDataUniformIndex, 0);

    if (lightingUniformIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(m_Program, lightingUniformIndex, 1);

    if (shadowDataUniformIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(m_Program, shadowDataUniformIndex, 2);

    GLint count;
    GLint buffSize;
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &count);
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffSize);

    GLsizei length;
    GLenum  type;
    GLchar  name[buffSize];
    int     textureUnit = 0;
    for (int i = 0; i < count; ++i)
    {
        glGetActiveUniform(m_Program, i, buffSize, &length, nullptr, &type, &name[0]);
        string nameStr(&name[0], length);

        GLint location = glGetUniformLocation(m_Program, &nameStr[0]);
        auto  uniform  = BaseUniform::Create(location, type, i);

        // TODO: correctly parse arrays

        if (uniform->GetType() == UniformType::UNKNOWN)
            fprintf(stderr, "Shader init error: Unknown OpenGL type for uniform %s: %d\n", &nameStr[0], type);
        else if (UniformTypeUtils::IsTexture(uniform->GetType()))
            m_TextureUnits[nameStr] = textureUnit++;

        m_Uniforms[nameStr] = std::move(uniform);
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_Program);
}

//region public methods

void Shader::Use() const
{
    m_CurrentShader = this;

    glUseProgram(m_Program);

    SetDefaultValues();
    for (const auto &pair: m_GlobalTextures)
        SetTextureUniform(pair.first, pair.second);
}

void Shader::SetUniform(const string &_name, const void *_data) const
{
    if (m_Uniforms.contains(_name) && m_Uniforms.at(_name) != nullptr)
        m_Uniforms.at(_name)->Set(_data);
}

void Shader::SetTextureUniform(const string &_name, const shared_ptr<Texture> &_texture) const
{
    if (_texture == nullptr || !m_TextureUnits.contains(_name))
        return;

    int unit = m_TextureUnits.at(_name);
    _texture->Bind(unit);
    SetUniform(_name, &unit);
}

void Shader::DetachCurrentShader()
{
    if (m_CurrentShader == nullptr)
        return;

    for (const auto &pair: m_CurrentShader->m_TextureUnits)
    {
        glActiveTexture(GL_TEXTURE0 + pair.second);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindSampler(pair.second, 0);
    }

    m_CurrentShader = nullptr;
    glUseProgram(0);
}

void Shader::SetGlobalTexture(const string &_name, shared_ptr<Texture> _texture)
{
    m_GlobalTextures[_name] = std::move(_texture);

    if (m_CurrentShader != nullptr)
        m_CurrentShader->SetTextureUniform(_name, m_GlobalTextures[_name]);
}

const shared_ptr<Shader> &Shader::GetFallbackShader() // NOLINT(misc-no-recursion)
{
    return ShaderLoader::GetFallbackShader();
}

//endregion

//region service methods

void Shader::SetDefaultValues() const
{
    auto white  = Texture2D::White();
    auto normal = Texture2D::Normal();

    for (const auto &pair: m_Uniforms)
    {
        if (!m_DefaultValues.contains(pair.first) || pair.second == nullptr)
            continue;

        string value = m_DefaultValues.at(pair.first);

        // TODO: add support for default values for other types
        if (pair.second->GetType() == UniformType::SAMPLER_2D)
        {
            if (value == "normal")
                SetTextureUniform(pair.first, normal);
            else if (value == "white")
                SetTextureUniform(pair.first, white);

            Vector4 st = Vector4(0, 0, 1, 1);
            SetUniform(pair.first + "ST", &st);
        }
        else if (pair.second->GetType() == UniformType::SAMPLER_2D_ARRAY)
        {
            // TODO: bind default texture array
        }
    }
}

//endregion