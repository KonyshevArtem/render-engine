#include "shader_pass.h"
#include "graphics_backend_api.h"
#include "enums/program_parameter.h"
#include "enums/uniform_block_parameter.h"

#include <vector>

int GetNameBufferSize(GraphicsBackendProgram program)
{
    int uniformNameLength;
    int uniformBlockNameLength;
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLength);
    GraphicsBackend::GetProgramParameter(program, ProgramParameter::ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &uniformBlockNameLength);
    return std::max(uniformNameLength, uniformBlockNameLength);
}

ShaderPass::ShaderPass(GraphicsBackendProgram program, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
                       std::unordered_map<std::string, std::string> &tags) :
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
        int binding;

        GraphicsBackend::GetActiveUniformBlockParameter(m_Program, i, UniformBlockParameter::BINDING, &binding);
        if (binding == 0)
        {
            binding = freeBlockBindings[freeBlockBindings.size() - 1];
            freeBlockBindings.pop_back();
        }

        GraphicsBackend::SetUniformBlockBinding(m_Program, i, binding);

        int uniformBlockNameSize;
        GraphicsBackend::GetActiveUniformBlockName(m_Program, i, uniformNameBuffer.size(), &uniformBlockNameSize, &uniformNameBuffer[0]);
        std::string uniformBlockName(uniformNameBuffer.begin(), uniformNameBuffer.begin() + uniformBlockNameSize);

        m_UniformBlockBindings[uniformBlockName] = binding;
    }

    int uniformCount;
    GraphicsBackend::GetProgramParameter(m_Program, ProgramParameter::ACTIVE_UNIFORMS, &uniformCount);

    TextureUnit textureUnit = TextureUnit::TEXTURE0;
    for (int i = 0; i < uniformCount; ++i)
    {
        int uniformSize;
        int uniformNameLength;
        UniformDataType uniformDataType;

        GraphicsBackend::GetActiveUniform(m_Program, i, uniformNameBuffer.size(), &uniformNameLength, &uniformSize, &uniformDataType, &uniformNameBuffer[0]);
        std::string uniformName(uniformNameBuffer.begin(), uniformNameBuffer.begin() + uniformNameLength);

        auto location = GraphicsBackend::GetUniformLocation(m_Program, &uniformName[0]);

        // TODO: correctly parse arrays

        if (UniformDataTypeUtils::IsTexture(uniformDataType))
        {
            m_TextureUnits[uniformName] = textureUnit;
            textureUnit = static_cast<TextureUnit>(static_cast<int>(textureUnit) + 1);
        }

        m_Uniforms[uniformName] = UniformInfo {uniformDataType, location, i};
    }
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
