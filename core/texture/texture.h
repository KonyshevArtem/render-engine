#ifndef OPENGL_STUDY_TEXTURE_H
#define OPENGL_STUDY_TEXTURE_H

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

class Texture
{
public:
    virtual ~Texture();

    virtual void Bind(int _unit) const = 0;

    inline unsigned int GetWidth() const
    {
        return m_Width;
    }

    inline unsigned int GetHeight() const
    {
        return m_Height;
    }

protected:
    unsigned int m_Width   = 0;
    unsigned int m_Height  = 0;
    GLuint       m_Texture = 0;
    GLuint       m_Sampler = 0;
};

#endif //OPENGL_STUDY_TEXTURE_H
