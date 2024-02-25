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
#include "shader/uniform_info/buffer_info.h"

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

void FillUniformBlockVariables(GraphicsBackendProgram program, int uniformBlockIndex, std::shared_ptr<BufferInfo> &buffer, std::vector<char> nameBuffer)
{
    int uniformsCount;
    GraphicsBackend::GetActiveUniformBlockParameter(program, uniformBlockIndex, UniformBlockParameter::ACTIVE_UNIFORMS, &uniformsCount);

    std::vector<int> uniformsIndices(uniformsCount);
    GraphicsBackend::GetActiveUniformBlockParameter(program, uniformBlockIndex, UniformBlockParameter::ACTIVE_UNIFORM_INDICES, &uniformsIndices[0]);

    for (unsigned int i = 0; i < uniformsCount; ++i)
    {
        int uniformNameLength;
        GraphicsBackend::GetActiveUniform(program, uniformsIndices[i], nameBuffer.size(), &uniformNameLength, nullptr, nullptr, &nameBuffer[0]);
        std::string uniformName(nameBuffer.begin(), nameBuffer.begin() + uniformNameLength);

        int uniformOffset;
        GraphicsBackend::GetActiveUniformsParameter(program, 1, reinterpret_cast<unsigned int *>(&uniformsIndices[i]), UniformParameter::OFFSET, &uniformOffset);

        buffer->AddVariable(uniformName, uniformOffset);
    }
}

void FillUniformBlocks(GraphicsBackendProgram program, std::unordered_map<std::string, std::shared_ptr<BufferInfo>> &buffers, std::vector<char> &nameBuffer)
{
    int uniformBlocksCount;
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_BLOCKS, &uniformBlocksCount);

    for (int i = 0; i < uniformBlocksCount; ++i)
    {
        GraphicsBackend::SetUniformBlockBinding(program, i, i);

        int nameSize;
        GraphicsBackend::GetActiveUniformBlockName(program, i, nameBuffer.size(), &nameSize, &nameBuffer[0]);
        std::string uniformBlockName(nameBuffer.begin(), nameBuffer.begin() + nameSize);

        int blockSize;
        GraphicsBackend::GetActiveUniformBlockParameter(program, i, UniformBlockParameter::DATA_SIZE, &blockSize);

        auto buffer = std::make_shared<BufferInfo>(BufferInfo::BufferType::UNIFORM, i, blockSize);
        FillUniformBlockVariables(program, i, buffer, nameBuffer);
        buffers[uniformBlockName] = std::move(buffer);
    }
}

void FillUniforms(GraphicsBackendProgram program, std::unordered_map<std::string, UniformInfo> &uniforms, std::vector<char> &nameBuffer)
{
    int uniformCount;
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORMS, &uniformCount);

    TextureUnit textureUnit = TextureUnit::TEXTURE0;
    for (unsigned int i = 0; i < uniformCount; ++i)
    {
        int blockIndex;
        GraphicsBackend::GetActiveUniformsParameter(program, 1, &i, UniformParameter::BLOCK_INDEX, &blockIndex);
        if (blockIndex >= 0)
        {
            continue;
        }

        UniformInfo uniformInfo{};

        int uniformNameLength;
        GraphicsBackend::GetActiveUniform(program, i, nameBuffer.size(), &uniformNameLength, nullptr, &uniformInfo.Type, &nameBuffer[0]);
        std::string uniformName(nameBuffer.begin(), nameBuffer.begin() + uniformNameLength);

        uniformInfo.Location = GraphicsBackend::GetUniformLocation(program, &uniformName[0]);

        // TODO: correctly parse arrays

        if (UniformDataTypeUtils::IsTexture(uniformInfo.Type))
        {
            uniformInfo.IsTexture = true;
            uniformInfo.TextureUnit = textureUnit;
            textureUnit = TextureUnitUtils::Next(textureUnit);
        }

        uniforms[uniformName] = uniformInfo;
    }
}

void FillShaderStorageBlockVariables(GraphicsBackendProgram program, int ssboIndex, std::shared_ptr<BufferInfo> &buffer)
{
    int variablesCount;
    auto variablesCountParameter = ProgramResourceParameter::NUM_ACTIVE_VARIABLES;
    GraphicsBackend::GetProgramResourceParameters(program, ProgramInterface::SHADER_STORAGE_BLOCK, ssboIndex, 1, &variablesCountParameter, 1, nullptr, &variablesCount);

    std::vector<int> variablesIndices(variablesCount);
    auto variablesIndicesParameter = ProgramResourceParameter::ACTIVE_VARIABLES;
    GraphicsBackend::GetProgramResourceParameters(program, ProgramInterface::SHADER_STORAGE_BLOCK, ssboIndex, 1, &variablesIndicesParameter, variablesCount, nullptr, &variablesIndices[0]);

    constexpr int variableParametersCount = 2;
    ProgramResourceParameter variableParameters[variableParametersCount] = {ProgramResourceParameter::NAME_LENGTH, ProgramResourceParameter::OFFSET};
    for(int i = 0; i < variablesCount; ++i)
    {
        int values[variableParametersCount];
        GraphicsBackend::GetProgramResourceParameters(program, ProgramInterface::BUFFER_VARIABLE, variablesIndices[i], variableParametersCount, &variableParameters[0], variableParametersCount, nullptr, &values[0]);

        std::vector<char> nameData(values[0]);
        GraphicsBackend::GetProgramResourceName(program, ProgramInterface::BUFFER_VARIABLE, variablesIndices[i], nameData.size(), nullptr, &nameData[0]);
        std::string name(nameData.begin(), nameData.end() - 1);

        buffer->AddVariable(name, values[1]);
    }
}

void FillShaderStorageBlocks(GraphicsBackendProgram program, std::unordered_map<std::string, std::shared_ptr<BufferInfo>> &buffers, std::vector<char> &nameBuffer)
{
    int blocksCount;
    GraphicsBackend::GetProgramInterfaceParameter(program, ProgramInterface::SHADER_STORAGE_BLOCK, ProgramInterfaceParameter::ACTIVE_RESOURCES, &blocksCount);

    for (int i = 0; i < blocksCount; ++i)
    {
        GraphicsBackend::SetShaderStorageBlockBinding(program, i, i);

        int nameSize;
        GraphicsBackend::GetProgramResourceName(program, ProgramInterface::SHADER_STORAGE_BLOCK, i, nameBuffer.size(), &nameSize, nameBuffer.data());
        std::string blockName(nameBuffer.begin(), nameBuffer.begin() + nameSize);

        int blockSize;
        auto blockSizeParameter = ProgramResourceParameter::BUFFER_DATA_SIZE;
        GraphicsBackend::GetProgramResourceParameters(program, ProgramInterface::SHADER_STORAGE_BLOCK, i, 1, &blockSizeParameter, 1, nullptr, &blockSize);

        auto buffer = std::make_shared<BufferInfo>(BufferInfo::BufferType::SHADER_STORAGE, i, blockSize);
        FillShaderStorageBlockVariables(program, i, buffer);
        buffers[blockName] = std::move(buffer);
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

    FillUniformBlocks(m_Program, m_Buffers, nameBuffer);
    FillUniforms(m_Program, m_Uniforms, nameBuffer);
    FillShaderStorageBlocks(m_Program, m_Buffers, nameBuffer);
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
