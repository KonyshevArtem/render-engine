#ifndef OPENGL_STUDY_TEXTURE_H
#define OPENGL_STUDY_TEXTURE_H

#include "OpenGL/gl3.h"

class Texture
{
public:
    unsigned int Width  = 0;
    unsigned int Height = 0;

    virtual void Bind(int _unit) const = 0;

protected:
    GLuint m_Texture = 0;
    GLuint m_Sampler = 0;
};

#endif //OPENGL_STUDY_TEXTURE_H
