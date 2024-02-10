#ifndef OPENGL_STUDY_DRAWABLE_GEOMETRY
#define OPENGL_STUDY_DRAWABLE_GEOMETRY

#include "enums/primitive_type.h"
#include "types/graphics_backend_vao.h"
#include "types/graphics_backend_buffer.h"

class DrawableGeometry
{
public:
    virtual ~DrawableGeometry();

    inline PrimitiveType GetPrimitiveType() const
    {
        return m_PrimitiveType;
    }

    inline bool HasIndexes() const
    {
        return m_HasIndices;
    }

    inline int GetElementsCount() const
    {
        return m_ElementsCount;
    }

    inline GraphicsBackendVAO GetVertexArrayObject() const
    {
        return m_VertexArrayObject;
    }

protected:
    DrawableGeometry(PrimitiveType primitiveType, int elementsCount, bool hasIndices);

private:
    GraphicsBackendVAO m_VertexArrayObject{};
    GraphicsBackendBuffer m_VertexBuffer{};

    PrimitiveType m_PrimitiveType;
    int m_ElementsCount;
    bool m_HasIndices;
};

#endif