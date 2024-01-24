#ifndef OPENGL_STUDY_DRAWABLE_GEOMETRY
#define OPENGL_STUDY_DRAWABLE_GEOMETRY

#include "enums/primitive_type.h"
#include "types/graphics_backend_vao.h"
#include "types/graphics_backend_buffer.h"

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
    GraphicsBackendVAO m_VertexArrayObject{};
    GraphicsBackendBuffer m_VertexBuffer{};
};

#endif