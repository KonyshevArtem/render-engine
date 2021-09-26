#ifndef OPENGL_STUDY_MESH_H
#define OPENGL_STUDY_MESH_H

#include "OpenGL/gl3.h"

class Mesh
{
public:
    void Init();

    virtual int GetTrianglesCount() = 0;

    virtual ~Mesh();

protected:
    virtual void *GetVertexData()     = 0;
    virtual long  GetVertexDataSize() = 0;

    virtual void *GetNormalsData()     = 0;
    virtual long  GetNormalsDataSize() = 0;

    virtual void *GetIndexData()     = 0;
    virtual long  GetIndexDataSize() = 0;

    virtual void *GetUVData()     = 0;
    virtual long  GetUVDataSize() = 0;

private:
    GLuint m_VertexArrayObject = 0;

    friend class RenderPass;
    friend class ShadowCasterPass;
    friend class SkyboxPass;
};


#endif //OPENGL_STUDY_MESH_H