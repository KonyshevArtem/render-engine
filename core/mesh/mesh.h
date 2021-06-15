#ifndef OPENGL_STUDY_MESH_H
#define OPENGL_STUDY_MESH_H

#define GL_SILENCE_DEPRECATION

#include "OpenGL/gl3.h"

class Mesh// NOLINT(cppcoreguidelines-pro-type-member-init)
{
public:
    const GLuint &GetVertexArrayObject() const;

    void Init();

    virtual int GetTrianglesCount() = 0;

protected:
    virtual void GetVertexes(const GLuint &vertexBuffer) = 0;
    virtual void GetColors(const GLuint &vertexBuffer)   = 0;
    virtual void GetNormals(const GLuint &vertexBuffer)  = 0;
    virtual void GetIndexes(const GLuint &indexBuffer)   = 0;

private:
    GLuint vertexArrayObject;
};


#endif//OPENGL_STUDY_MESH_H
