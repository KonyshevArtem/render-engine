#include "uniform_block.h"
#include "debug.h"
#include "shader/shader.h"
#include "shader/shader_pass/shader_pass.h"
#include "graphics_backend_api.h"
#include "enums/uniform_block_parameter.h"
#include "enums/uniform_parameter.h"
#include "enums/buffer_bind_target.h"
#include "enums/buffer_usage_hint.h"

UniformBlock::UniformBlock(const Shader &_shader, std::string _blockName) :
    m_Name(std::move(_blockName))
{
    auto &pass = _shader.m_Passes.at(0);

    int blockIndex;
    if (!GraphicsBackend::TryGetUniformBlockIndex(pass->GetProgram(), m_Name.c_str(), &blockIndex))
        return;

    int uniformCount;
    GraphicsBackend::GetActiveUniformBlockParameter(pass->GetProgram(), blockIndex, UniformBlockParameter::ACTIVE_UNIFORMS, &uniformCount);

    std::vector<int> uniformIndexes(uniformCount);
    GraphicsBackend::GetActiveUniformBlockParameter(pass->GetProgram(), blockIndex, UniformBlockParameter::ACTIVE_UNIFORM_INDICES, &uniformIndexes[0]);

    std::vector<int> uniformOffsets(uniformCount);
    GraphicsBackend::GetActiveUniformsParameter(pass->GetProgram(), uniformCount, reinterpret_cast<const unsigned int *>(&uniformIndexes[0]), UniformParameter::OFFSET, &uniformOffsets[0]);

    std::unordered_map<int, int> indexToOffset;
    for (int i = 0; i < uniformCount; ++i)
        indexToOffset[uniformIndexes[i]] = uniformOffsets[i];

    for (const auto &pair: pass->GetUniforms())
    {
        if (indexToOffset.contains(pair.second.Index))
            m_UniformOffsets[pair.first] = indexToOffset[pair.second.Index];
    }

    int blockSize;
    GraphicsBackend::GetActiveUniformBlockParameter(pass->GetProgram(), blockIndex, UniformBlockParameter::DATA_SIZE, &blockSize);

    GraphicsBackend::GenerateBuffers(1, &m_Buffer);
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, m_Buffer);
    GraphicsBackend::SetBufferData(BufferBindTarget::UNIFORM_BUFFER, blockSize, nullptr, BufferUsageHint::DYNAMIC_DRAW);
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, GraphicsBackendBuffer::NONE);

    m_Data.resize(blockSize);
}

UniformBlock::~UniformBlock()
{
    GraphicsBackend::DeleteBuffers(1, &m_Buffer);
}

void UniformBlock::Bind(int binding) const
{
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, m_Buffer);
    GraphicsBackend::BindBufferRange(BufferBindTarget::UNIFORM_BUFFER, binding, m_Buffer, 0, m_Data.size());
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, GraphicsBackendBuffer::NONE);
}

void UniformBlock::SetUniform(const std::string &_name, const void *_data, unsigned long _size)
{
    if (!m_UniformOffsets.contains(_name))
    {
        Debug::LogErrorFormat("[UniformBlock] %1% does not have %2% uniform", {m_Name, _name});
        return;
    }

    auto offset = m_UniformOffsets[_name];
    memcpy(m_Data.data() + offset, _data, _size);
}

void UniformBlock::UploadData() const
{
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, m_Buffer);
    GraphicsBackend::SetBufferSubData(BufferBindTarget::UNIFORM_BUFFER, 0, static_cast<long>(m_Data.size()), m_Data.data());
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, GraphicsBackendBuffer::NONE);
}
