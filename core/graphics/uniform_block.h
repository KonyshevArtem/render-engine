#ifndef OPENGL_STUDY_UNIFORM_BLOCK_H
#define OPENGL_STUDY_UNIFORM_BLOCK_H

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <memory>
#include <string>
#include <unordered_map>

class Shader;

class UniformBlock
{
public:
    UniformBlock(const Shader &_shader, std::string _blockName, unsigned int _index);
    ~UniformBlock();

    void Bind() const;
    void SetUniform(const std::string &_name, const void *_data, unsigned long _size);
    void UploadData() const;

private:
    UniformBlock(const UniformBlock &) = delete;
    UniformBlock(UniformBlock &&)      = delete;

    UniformBlock &operator()(const UniformBlock &) = delete;
    UniformBlock &operator()(UniformBlock &&)      = delete;

    std::string                            m_Name;
    GLuint                                 m_Buffer = 0;
    unsigned int                           m_BindIndex;
    std::vector<uint8_t>                   m_Data;
    std::unordered_map<std::string, GLint> m_UniformOffsets;
};

#endif //OPENGL_STUDY_UNIFORM_BLOCK_H
