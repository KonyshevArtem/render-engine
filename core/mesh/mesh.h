#ifndef OPENGL_STUDY_MESH_H
#define OPENGL_STUDY_MESH_H

#define GL_SILENCE_DEPRECATION

#include "OpenGL/gl3.h"

class Mesh // NOLINT(cppcoreguidelines-pro-type-member-init)
{
public:
    const GLuint &GetVertexArrayObject() const;

    void Init();

    virtual int GetTrianglesCount() = 0;

    virtual ~Mesh();

protected:
    virtual void *GetVertexData()     = 0;
    virtual long  GetVertexDataSize() = 0;

    virtual void *GetColorsData()     = 0;
    virtual long  GetColorsDataSize() = 0;

    virtual void *GetNormalsData()     = 0;
    virtual long  GetNormalsDataSize() = 0;

    virtual void *GetIndexData()     = 0;
    virtual long  GetIndexDataSize() = 0;

    virtual void *GetUVData()     = 0;
    virtual long  GetUVDataSize() = 0;

private:
    GLuint vertexArrayObject;
};


#endif //OPENGL_STUDY_MESH_H
