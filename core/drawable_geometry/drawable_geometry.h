#ifndef OPENGL_STUDY_DRAWABLE_GEOMETRY
#define OPENGL_STUDY_DRAWABLE_GEOMETRY

#include "graphics_backend_api.h"

class DrawableGeometry
{
public:
    virtual ~DrawableGeometry();

    virtual PrimitiveType GetPrimitiveType() const = 0;
    virtual bool HasIndexes() const = 0;
    virtual int GetElementsCount() const = 0;

    inline GraphicsBackendVAO GetVertexArrayObject() const
    {
        return m_VertexArrayObject;
    }

protected:
    DrawableGeometry();

private:
    GraphicsBackendVAO m_VertexArrayObject = 0;
    GraphicsBackendBuffer m_VertexBuffer = 0;
};

#endif