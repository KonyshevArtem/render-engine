#include "uniform_block.h"
#include "core_debug/debug.h"
#include "shader/shader.h"
#include "shader/uniform_info/uniform_info.h"
#include <unordered_map>
#include <vector>

UniformBlock::UniformBlock(const Shader &_shader, std::string _blockName, unsigned int _index) :
    m_Name(std::move(_blockName))
{
    auto &pass = _shader.m_Passes.at(0);

    auto blockIndex = glGetUniformBlockIndex(pass.Program, m_Name.c_str());
    if (blockIndex == GL_INVALID_INDEX)
        return;

    GLint uniformCount;
    glGetActiveUniformBlockiv(pass.Program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);

    std::vector<GLint> uniformIndexes(uniformCount);
    glGetActiveUniformBlockiv(pass.Program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &uniformIndexes[0]);

    std::vector<GLint> uniformOffsets(uniformCount);
    glGetActiveUniformsiv(pass.Program, uniformCount, reinterpret_cast<const GLuint *>(&uniformIndexes[0]), GL_UNIFORM_OFFSET, &uniformOffsets[0]);

    std::unordered_map<GLint, GLint> indexToOffset;
    for (int i = 0; i < uniformCount; ++i)
        indexToOffset[uniformIndexes[i]] = uniformOffsets[i];

    for (const auto &pair: pass.Uniforms)
    {
        if (indexToOffset.contains(pair.second.Index))
            m_UniformOffsets[pair.first] = indexToOffset[pair.second.Index];
    }

    GLint blockSize;
    glGetActiveUniformBlockiv(pass.Program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

    glGenBuffers(1, &m_Buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, _index, m_Buffer, 0, blockSize);
}

UniformBlock::~UniformBlock()
{
    glDeleteBuffers(1, &m_Buffer);
}

void UniformBlock::SetUniform(const std::string &_name, const void *_data, unsigned long _size)
{
    if (!m_UniformOffsets.contains(_name))
    {
        Debug::LogErrorFormat("[UniformBlock] %1% does not have %2% uniform", {m_Name, _name});
        return;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, m_UniformOffsets[_name], static_cast<GLsizei>(_size), _data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
