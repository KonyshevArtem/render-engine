#include "material.h"
#include "shader/shader.h"
#include "texture/texture.h"
#include "shader/shader_pass/shader_pass.h"
#include "shader/uniform_info/uniform_block_info.h"
#include "graphics_buffer/graphics_buffer.h"

#include <utility>

Material::Material(std::shared_ptr<Shader> _shader) :
        m_Shader(std::move(_shader))
{
    int passesCount = m_Shader->PassesCount();
    for (int i = 0; i < passesCount; ++i)
    {
        auto pass = m_Shader->GetPass(i);
        const auto &uniforms = pass->GetUniforms();
        const auto &uniformBlocks = pass->GetUniformBlocks();

        auto it = uniformBlocks.find("PerMaterialData");
        if (it != uniformBlocks.end())
        {
            const auto &uniformBlockInfo = it->second;
            auto uniformBlock = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, uniformBlockInfo.Size, BufferUsageHint::DYNAMIC_DRAW);
            m_UniformBlocks.push_back(uniformBlock);

            std::unordered_map<std::string, UniformInfo> perMaterialDataUniforms;
            for (const auto &pair: uniforms)
            {
                const auto &uniformInfo = pair.second;
                if (uniformInfo.BlockIndex == uniformBlockInfo.Index)
                {
                    perMaterialDataUniforms[pair.first] = uniformInfo;
                }
            }
            m_PerMaterialDataUniforms.push_back(perMaterialDataUniforms);
        }
        else
        {
            m_UniformBlocks.push_back(nullptr);
            m_PerMaterialDataUniforms.emplace_back(0);
        }
    }
}

void Material::SetTexture(const std::string &name, std::shared_ptr<Texture> texture)
{
    m_PropertyBlock.SetTexture(name, texture);
    if (texture == nullptr)
    {
        return;
    }

    auto scaleTransformPropName = name + "_ST";
    if (!m_PropertyBlock.HasProperty(scaleTransformPropName))
    {
        SetVector(scaleTransformPropName, {0, 0, 1, 1});
    }

    int width = texture->GetWidth();
    int height = texture->GetHeight();
    SetVector(name + "_TexelSize", {static_cast<float>(width), static_cast<float>(height), 1.0f / width, 1.0f / height});
}

void Material::SetVector(const std::string &name, const Vector4 &value)
{
    m_PropertyBlock.SetVector(name, value);
    SetDataToUniformBlocks(name, &value, sizeof(Vector4));
}

void Material::SetFloat(const std::string &name, float value)
{
    m_PropertyBlock.SetFloat(name, value);
    SetDataToUniformBlocks(name, &value, sizeof(float));
}

void Material::SetMatrix(const std::string &name, const Matrix4x4 &value)
{
    m_PropertyBlock.SetMatrix(name, value);
    SetDataToUniformBlocks(name, &value, sizeof(Matrix4x4));
}

void Material::SetDataToUniformBlocks(const std::string &name, const void *data, uint64_t size)
{
    int passesCount = m_Shader->PassesCount();
    for (int i = 0; i < passesCount; ++i)
    {
        const auto &uniforms = m_PerMaterialDataUniforms[i];

        auto it = uniforms.find(name);
        if (it != uniforms.end())
        {
            m_UniformBlocks[i]->SetData(data, it->second.BlockOffset, size);
        }
    }
}
