#ifndef OPENGL_STUDY_UNIFORM_BLOCK_H
#define OPENGL_STUDY_UNIFORM_BLOCK_H

#include "types/graphics_backend_buffer.h"

#include <string>
#include <unordered_map>
#include <vector>

class Shader;

class UniformBlock
{
public:
    UniformBlock(const Shader &_shader, std::string _blockName);
    ~UniformBlock();

    void Bind(int binding) const;
    void SetUniform(const std::string &_name, const void *_data, unsigned long _size);
    void UploadData() const;

    inline const std::string &GetName() const
    {
        return m_Name;
    }

    UniformBlock(const UniformBlock &) = delete;
    UniformBlock(UniformBlock &&)      = delete;

    UniformBlock &operator()(const UniformBlock &) = delete;
    UniformBlock &operator()(UniformBlock &&)      = delete;

private:
    std::string                            m_Name;
    GraphicsBackendBuffer                  m_Buffer{};
    std::vector<uint8_t>                   m_Data;
    std::unordered_map<std::string, int>   m_UniformOffsets;
};

#endif //OPENGL_STUDY_UNIFORM_BLOCK_H
