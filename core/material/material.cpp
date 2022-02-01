#include "material.h"
#include "../../math/vector4/vector4.h"
#include "../shader/shader.h"
#include "../texture/texture.h"
#include <utility>

Material::Material(std::shared_ptr<Shader> _shader) :
    m_Shader(std::move(_shader))
{
}

void Material::SetTexture(const std::string &_name, std::shared_ptr<Texture> _value)
{
    m_Textures[_name]        = std::move(_value);
    m_Vectors4[_name + "ST"] = Vector4(0, 0, 1, 1);
}

const std::shared_ptr<Texture> Material::GetTexture(const std::string &_name) const
{
    return m_Textures.contains(_name) ? m_Textures.at(_name) : nullptr;
}

void Material::SetVector4(const std::string &_name, const Vector4 &_value)
{
    m_Vectors4[_name] = _value;
}

const Vector4 Material::GetVector4(const std::string &_name) const
{
    return m_Vectors4.contains(_name) ? m_Vectors4.at(_name) : Vector4::Zero();
}

void Material::SetFloat(const std::string &_name, float _value)
{
    m_Floats[_name] = _value;
}

float Material::GetFloat(const std::string &_name) const
{
    return m_Floats.contains(_name) ? m_Floats.at(_name) : 0;
}

const std::shared_ptr<Shader> &Material::GetShader() const
{
    return m_Shader;
}

void Material::TransferUniforms() const
{
    for (const auto &pair: m_Textures)
    {
        if (pair.second != nullptr)
            Shader::SetTextureUniform(pair.first, *pair.second);
    }
    for (const auto &pair: m_Vectors4)
        Shader::SetUniform(pair.first, &pair.second);
    for (const auto &pair: m_Floats)
        Shader::SetUniform(pair.first, &pair.second);
}
