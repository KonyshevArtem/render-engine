#include "shader_pass.h"
#include "graphics_backend_api.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "types/graphics_backend_uniform_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "global_constants.h"

#include <vector>

std::shared_ptr<Texture> GetTextureByLiteralAndType(const std::string &literal, UniformDataType dataType)
{
    if (literal == "white")
    {
        switch (dataType)
        {
            case UniformDataType::SAMPLER_2D:
                return Texture2D::White();
            case UniformDataType::SAMPLER_CUBE:
                return Cubemap::White();
        }
    }

    if (literal == "black")
    {
        switch (dataType)
        {
            case UniformDataType::SAMPLER_CUBE:
                return Cubemap::Black();
        }
    }

    if (literal == "normal")
    {
        switch (dataType)
        {
            case UniformDataType::SAMPLER_2D:
                return Texture2D::Normal();
        }
    }

    return nullptr;
}

void FillDefaultValuesPropertyBlock(const std::unordered_map<std::string, std::string> &defaultValues,
                                    const std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms,
                                    PropertyBlock &propertyBlock)
{
    for (const auto &pair: defaultValues)
    {
        auto &uniformName = pair.first;

        auto it = uniforms.find(uniformName);
        if (it == uniforms.end())
            continue;

        auto &defaultValueLiteral = pair.second;
        auto uniformDataType = it->second.Type;

        if (UniformDataTypeUtils::IsTexture(uniformDataType))
        {
            auto texture = GetTextureByLiteralAndType(defaultValueLiteral, uniformDataType);
            if (texture != nullptr)
            {
                propertyBlock.SetTexture(uniformName, texture);
            }
        }
        else
        {
            // TODO: add support for default values for other types
        }
    }
}

ShaderPass::ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes,
                       BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo, TextureInternalFormat colorFormat, TextureInternalFormat depthFormat,
                       std::unordered_map<std::string, std::string> &tags, const std::unordered_map<std::string, std::string> &defaultValues) :
        m_Shaders(std::move(shaders)),
        m_BlendInfo(blendInfo),
        m_CullInfo(cullInfo),
        m_DepthInfo(depthInfo),
        m_ColorFormat(colorFormat),
        m_DepthFormat(depthFormat),
        m_Tags(std::move(tags))
{
    auto program = GraphicsBackend::Current()->CreateProgram(m_Shaders, m_ColorFormat, m_DepthFormat, vertexAttributes);
    m_Programs[VertexAttributes::GetHash(vertexAttributes)] = program;

    GraphicsBackend::Current()->IntrospectProgram(program, m_Uniforms, m_Buffers);
    FillDefaultValuesPropertyBlock(defaultValues, m_Uniforms, m_DefaultValuesBlock);
}

ShaderPass::~ShaderPass()
{
    for (auto &pair : m_Programs)
    {
        GraphicsBackend::Current()->DeleteProgram(pair.second);
    }

    for (auto &shader : m_Shaders)
    {
        GraphicsBackend::Current()->DeleteShader(shader);
    }
}

const GraphicsBackendProgram &ShaderPass::GetProgram(const VertexAttributes &vertexAttributes)
{
    if (!GraphicsBackend::Current()->RequireStrictPSODescriptor() && !m_Programs.empty())
    {
        return m_Programs.begin()->second;
    }

    auto hash = vertexAttributes.GetHash();
    auto it = m_Programs.find(hash);
    if (it != m_Programs.end())
    {
        return it->second;
    }

    auto program = GraphicsBackend::Current()->CreateProgram(m_Shaders, m_ColorFormat, m_DepthFormat, vertexAttributes.GetAttributes());
    m_Programs[hash] = program;
    return program;
}

std::string ShaderPass::GetTagValue(const std::string &tag) const
{
    auto it = m_Tags.find(tag);
    return it != m_Tags.end() ? it->second : "";
}
