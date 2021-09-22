#ifndef OPENGL_STUDY_UNIFORM_BLOCK_H
#define OPENGL_STUDY_UNIFORM_BLOCK_H

#include "../shader/shader.h"
#include "OpenGL/gl3.h"
#include "memory"
#include "string"
#include "unordered_map"

using namespace std;

class UniformBlock
{
public:
    UniformBlock(const shared_ptr<Shader> &_shader, const string &_blockName, unsigned int _index);
    ~UniformBlock();

    void SetUniform(const string &_name, const void *_data, unsigned long _size);

private:
    string                       m_Name;
    GLuint                       m_Buffer = 0;
    unordered_map<string, GLint> m_UniformOffsets;
};

#endif //OPENGL_STUDY_UNIFORM_BLOCK_H
