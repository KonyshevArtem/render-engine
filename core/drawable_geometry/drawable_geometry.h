#ifndef OPENGL_STUDY_DRAWABLE_GEOMETRY
#define OPENGL_STUDY_DRAWABLE_GEOMETRY

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

class DrawableGeometry
{
public:
    virtual ~DrawableGeometry();

    virtual GLenum  GetGeometryType() const  = 0;
    virtual bool    HasIndexes() const       = 0;
    virtual GLsizei GetElementsCount() const = 0;

    inline GLuint GetVertexArrayObject() const
    {
        return m_VertexArrayObject;
    }

protected:
    DrawableGeometry();

    inline GLuint GetVertexBuffer() const
    {
        return m_VertexBuffer;
    }

private:
    GLuint m_VertexArrayObject = 0;
    GLuint m_VertexBuffer      = 0;
};

#endif