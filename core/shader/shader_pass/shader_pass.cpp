#include "shader_pass.h"
#include "graphics_backend_api.h"
#include "enums/program_parameter.h"
#include "enums/uniform_parameter.h"
#include "enums/uniform_block_parameter.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "shader/uniform_info/uniform_info.h"
#include "shader/uniform_info/uniform_block_info.h"

#include <vector>

int GetNameBufferSize(GraphicsBackendProgram program)
{
    int uniformNameLength;
    int uniformBlockNameLength;
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLength);
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &uniformBlockNameLength);
    return std::max(uniformNameLength, uniformBlockNameLength);
}

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
                                    const std::unordered_map<std::string, UniformInfo> &uniforms,
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

ShaderPass::ShaderPass(GraphicsBackendProgram program, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
                       std::unordered_map<std::string, std::string> &tags, const std::unordered_map<std::string, std::string> &defaultValues) :
        m_Program(program),
        m_BlendInfo(blendInfo),
        m_CullInfo(cullInfo),
        m_DepthInfo(depthInfo),
        m_Tags(std::move(tags))
{
    std::vector<char> uniformNameBuffer(GetNameBufferSize(m_Program));

    int uniformBlocksCount;
    GraphicsBackend::GetProgramParameter(m_Program, ProgramParameter::ACTIVE_UNIFORM_BLOCKS, &uniformBlocksCount);

    std::vector<int> freeBlockBindings(uniformBlocksCount);
    for (int i = 0; i < uniformBlocksCount; ++i)
    {
        freeBlockBindings[i] = i;
    }

    for (int i = 0; i < uniformBlocksCount; ++i)
    {
        UniformBlockInfo uniformBlockInfo{i};

        GraphicsBackend::GetActiveUniformBlockParameter(m_Program, i, UniformBlockParameter::BINDING, &uniformBlockInfo.Binding);
        if (uniformBlockInfo.Binding == 0)
        {
            uniformBlockInfo.Binding = freeBlockBindings[freeBlockBindings.size() - 1];
            freeBlockBindings.pop_back();
        }

        GraphicsBackend::SetUniformBlockBinding(m_Program, i, uniformBlockInfo.Binding);

        int uniformBlockNameSize;
        GraphicsBackend::GetActiveUniformBlockName(m_Program, i, uniformNameBuffer.size(), &uniformBlockNameSize, &uniformNameBuffer[0]);
        std::string uniformBlockName(uniformNameBuffer.begin(), uniformNameBuffer.begin() + uniformBlockNameSize);

        GraphicsBackend::GetActiveUniformBlockParameter(m_Program, i, UniformBlockParameter::DATA_SIZE, &uniformBlockInfo.Size);

        m_UniformBlockBindings[uniformBlockName] = uniformBlockInfo;
    }

    int uniformCount;
    GraphicsBackend::GetProgramParameter(m_Program, ProgramParameter::ACTIVE_UNIFORMS, &uniformCount);

    TextureUnit textureUnit = TextureUnit::TEXTURE0;
    for (unsigned int i = 0; i < uniformCount; ++i)
    {
        int uniformSize;
        int uniformNameLength;
        UniformInfo uniformInfo{};

        GraphicsBackend::GetActiveUniform(m_Program, i, uniformNameBuffer.size(), &uniformNameLength, &uniformSize, &uniformInfo.Type, &uniformNameBuffer[0]);
        std::string uniformName(uniformNameBuffer.begin(), uniformNameBuffer.begin() + uniformNameLength);

        uniformInfo.Location = GraphicsBackend::GetUniformLocation(m_Program, &uniformName[0]);

        // TODO: correctly parse arrays

        if (UniformDataTypeUtils::IsTexture(uniformInfo.Type))
        {
            uniformInfo.IsTexture = true;
            uniformInfo.TextureUnit = textureUnit;
            textureUnit = TextureUnitUtils::Next(textureUnit);
        }

        GraphicsBackend::GetActiveUniformsParameter(m_Program, 1, &i, UniformParameter::BLOCK_INDEX, &uniformInfo.BlockIndex);
        GraphicsBackend::GetActiveUniformsParameter(m_Program, 1, &i, UniformParameter::OFFSET, &uniformInfo.BlockOffset);

        m_Uniforms[uniformName] = uniformInfo;
    }

    FillDefaultValuesPropertyBlock(defaultValues, m_Uniforms, m_DefaultValuesBlock);
}

ShaderPass::~ShaderPass()
{
    GraphicsBackend::DeleteProgram(m_Program);
}

std::string ShaderPass::GetTagValue(const std::string &tag) const
{
    auto it = m_Tags.find(tag);
    return it != m_Tags.end() ? it->second : "";
}
