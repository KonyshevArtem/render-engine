#include "uniform_block.h"
#include "debug.h"
#include "shader/shader.h"
#include "shader/uniform_info/uniform_info.h"
#include <unordered_map>
#include <vector>

UniformBlock::UniformBlock(const Shader &_shader, std::string _blockName, unsigned int _index) :
    m_Name(std::move(_blockName)), m_BindIndex(_index)
{
    auto &pass = _shader.m_Passes.at(0);

    auto blockIndex = CHECK_GL(glGetUniformBlockIndex(pass.Program, m_Name.c_str()));
    if (blockIndex == GL_INVALID_INDEX)
        return;

    GLint uniformCount;
    CHECK_GL(glGetActiveUniformBlockiv(pass.Program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount));

    std::vector<GLint> uniformIndexes(uniformCount);
    CHECK_GL(glGetActiveUniformBlockiv(pass.Program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &uniformIndexes[0]));

    std::vector<GLint> uniformOffsets(uniformCount);
    CHECK_GL(glGetActiveUniformsiv(pass.Program, uniformCount, reinterpret_cast<const GLuint *>(&uniformIndexes[0]), GL_UNIFORM_OFFSET, &uniformOffsets[0]));

    std::unordered_map<GLint, GLint> indexToOffset;
    for (int i = 0; i < uniformCount; ++i)
        indexToOffset[uniformIndexes[i]] = uniformOffsets[i];

    for (const auto &pair: pass.Uniforms)
    {
        if (indexToOffset.contains(pair.second.Index))
            m_UniformOffsets[pair.first] = indexToOffset[pair.second.Index];
    }

    GLint blockSize;
    CHECK_GL(glGetActiveUniformBlockiv(pass.Program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize));

    CHECK_GL(glGenBuffers(1, &m_Buffer));
    CHECK_GL(glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer));
    CHECK_GL(glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_DYNAMIC_DRAW));
    CHECK_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    m_Data.resize(blockSize);
}

UniformBlock::~UniformBlock()
{
    CHECK_GL(glDeleteBuffers(1, &m_Buffer));
}

void UniformBlock::Bind() const
{
    CHECK_GL(glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer));
    CHECK_GL(glBindBufferRange(GL_UNIFORM_BUFFER, m_BindIndex, m_Buffer, 0, m_Data.size()));
    CHECK_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
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
    CHECK_GL(glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer));
    CHECK_GL(glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<GLsizei>(m_Data.size()), m_Data.data()));
    CHECK_GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
