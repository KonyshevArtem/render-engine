#include "shader_pass.h"
#include "graphics_backend_api.h"
#include "enums/program_parameter.h"
#include "enums/uniform_parameter.h"
#include "enums/uniform_block_parameter.h"
#include "enums/program_interface.h"
#include "enums/program_interface_parameter.h"
#include "enums/program_resource_parameter.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "shader/uniform_info/uniform_info.h"
#include "shader/uniform_info/uniform_block_info.h"
#include "shader/uniform_info/shader_storage_block_info.h"

#include <vector>

int GetNameBufferSize(GraphicsBackendProgram program)
{
    int uniformNameLength;
    int uniformBlockNameLength;
    int shaderStorageBlockNameLength;
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLength);
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &uniformBlockNameLength);
    GraphicsBackend::GetProgramInterfaceParameter(program, ProgramInterface::SHADER_STORAGE_BLOCK, ProgramInterfaceParameter::MAX_NAME_LENGTH, &shaderStorageBlockNameLength);
    return std::max(uniformNameLength, std::max(uniformBlockNameLength, shaderStorageBlockNameLength));
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

void FillUniformBlockInfo(GraphicsBackendProgram program, std::unordered_map<std::string, UniformBlockInfo> &uniformBlocks, std::vector<char> &nameBuffer)
{
    int uniformBlocksCount;
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_BLOCKS, &uniformBlocksCount);

    for (int i = 0; i < uniformBlocksCount; ++i)
    {
        UniformBlockInfo uniformBlockInfo{i, i};

        GraphicsBackend::SetUniformBlockBinding(program, i, uniformBlockInfo.Binding);

        int uniformBlockNameSize;
        GraphicsBackend::GetActiveUniformBlockName(program, i, nameBuffer.size(), &uniformBlockNameSize, &nameBuffer[0]);
        std::string uniformBlockName(nameBuffer.begin(), nameBuffer.begin() + uniformBlockNameSize);

        GraphicsBackend::GetActiveUniformBlockParameter(program, i, UniformBlockParameter::DATA_SIZE, &uniformBlockInfo.Size);

        uniformBlocks[uniformBlockName] = uniformBlockInfo;
    }
}

void FillUniformInfo(GraphicsBackendProgram program, std::unordered_map<std::string, UniformInfo> &uniforms, std::vector<char> &nameBuffer)
{
    int uniformCount;
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORMS, &uniformCount);

    TextureUnit textureUnit = TextureUnit::TEXTURE0;
    for (unsigned int i = 0; i < uniformCount; ++i)
    {
        int uniformSize;
        int uniformNameLength;
        UniformInfo uniformInfo{};

        GraphicsBackend::GetActiveUniform(program, i, nameBuffer.size(), &uniformNameLength, &uniformSize, &uniformInfo.Type, &nameBuffer[0]);
        std::string uniformName(nameBuffer.begin(), nameBuffer.begin() + uniformNameLength);

        uniformInfo.Location = GraphicsBackend::GetUniformLocation(program, &uniformName[0]);

        // TODO: correctly parse arrays

        if (UniformDataTypeUtils::IsTexture(uniformInfo.Type))
        {
            uniformInfo.IsTexture = true;
            uniformInfo.TextureUnit = textureUnit;
            textureUnit = TextureUnitUtils::Next(textureUnit);
        }

        GraphicsBackend::GetActiveUniformsParameter(program, 1, &i, UniformParameter::BLOCK_INDEX, &uniformInfo.BlockIndex);
        GraphicsBackend::GetActiveUniformsParameter(program, 1, &i, UniformParameter::OFFSET, &uniformInfo.BlockOffset);

        uniforms[uniformName] = uniformInfo;
    }
}

void FillShaderStorageBlocks(GraphicsBackendProgram program, std::unordered_map<std::string, ShaderStorageBlockInfo> &shaderStorageBlocks, std::vector<char> &nameBuffer)
{
    int blocksCount;
    GraphicsBackend::GetProgramInterfaceParameter(program, ProgramInterface::SHADER_STORAGE_BLOCK, ProgramInterfaceParameter::ACTIVE_RESOURCES, &blocksCount);

    for (int i = 0; i < blocksCount; ++i)
    {
        GraphicsBackend::SetShaderStorageBlockBinding(program, i, i);

        int nameSize;
        GraphicsBackend::GetProgramResourceName(program, ProgramInterface::SHADER_STORAGE_BLOCK, i, nameBuffer.size(), &nameSize, nameBuffer.data());
        std::string blockName(nameBuffer.begin(), nameBuffer.begin() + nameSize);

        shaderStorageBlocks[blockName] = {i, i};
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
    std::vector<char> nameBuffer(GetNameBufferSize(m_Program));

    FillUniformBlockInfo(m_Program, m_UniformBlocks, nameBuffer);
    FillUniformInfo(m_Program, m_Uniforms, nameBuffer);
    FillShaderStorageBlocks(m_Program, m_ShaderStorageBlocks, nameBuffer);
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
