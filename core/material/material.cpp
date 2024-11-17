#include "material.h"
#include "shader/shader.h"
#include "texture/texture.h"
#include "graphics_buffer/graphics_buffer_wrapper.h"
#include "global_constants.h"

#include <utility>

Material::Material(std::shared_ptr<Shader> shader, const std::string& name) :
        m_Shader(std::move(shader))
{
    m_PerMaterialDataBufferWrapper = std::make_shared<GraphicsBufferWrapper>(m_Shader, GlobalConstants::PerMaterialDataBufferName, name);
}

void Material::SetTexture(const std::string &name, std::shared_ptr<Texture> texture)
{
    m_Textures[name] = texture;
    if (texture == nullptr)
    {
        return;
    }

    SetVector(name + "_ST", {0, 0, 1, 1});

    int width = texture->GetWidth();
    int height = texture->GetHeight();
    SetVector(name + "_TexelSize", {static_cast<float>(width), static_cast<float>(height), 1.0f / width, 1.0f / height});
}

void Material::SetVector(const std::string &name, const Vector4 &value)
{
    SetDataToConstantBuffer(name, &value, sizeof(Vector4));
}

void Material::SetFloat(const std::string &name, float value)
{
    SetDataToConstantBuffer(name, &value, sizeof(float));
}

void Material::SetMatrix(const std::string &name, const Matrix4x4 &value)
{
    SetDataToConstantBuffer(name, &value, sizeof(Matrix4x4));
}

void Material::SetInt(const std::string &name, int value)
{
    SetDataToConstantBuffer(name, &value, sizeof(int));
}

void Material::SetDataToConstantBuffer(const std::string &name, const void *data, uint64_t size)
{
    int passesCount = m_Shader->PassesCount();
    for (int i = 0; i < passesCount; ++i)
    {
        m_PerMaterialDataBufferWrapper->TrySetVariable(name, data, size);
    }
}

std::shared_ptr<GraphicsBuffer> Material::GetPerMaterialDataBuffer(int pass) const
{
    return m_PerMaterialDataBufferWrapper->GetBuffer(pass);
}
