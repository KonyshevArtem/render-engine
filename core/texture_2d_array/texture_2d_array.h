#ifndef OPENGL_STUDY_TEXTURE_2D_ARRAY_H
#define OPENGL_STUDY_TEXTURE_2D_ARRAY_H

#include "OpenGL/gl3.h"
#include "memory"
#include "vector"

using namespace std;

class Texture2DArray
{
public:
    unsigned int Width;
    unsigned int Height;
    unsigned int Count;

    static shared_ptr<Texture2DArray> ShadowMapArray(unsigned int _size, unsigned int _count);

    ~Texture2DArray();

private:
    void Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode);

    GLuint                m_Texture;
    GLuint                m_Sampler;
    vector<unsigned char> m_Data;

    friend class RenderPass;
    friend class ShadowCasterPass;
};

#endif //OPENGL_STUDY_TEXTURE_2D_ARRAY_H
