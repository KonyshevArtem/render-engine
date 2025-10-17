#ifndef RENDER_ENGINE_DRAWABLE_GEOMETRY
#define RENDER_ENGINE_DRAWABLE_GEOMETRY

#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_buffer.h"
#include "drawable_geometry/vertex_attributes/vertex_attributes.h"

class DrawableGeometry
{
public:
    virtual ~DrawableGeometry();

    inline PrimitiveType GetPrimitiveType() const
    {
        return m_PrimitiveType;
    }

    inline IndicesDataType GetIndicesDataType() const
    {
        return m_IndicesDataType;
    }

    inline bool HasIndexes() const
    {
        return m_HasIndices;
    }

    inline int GetElementsCount() const
    {
        return m_ElementsCount;
    }

    inline const GraphicsBackendGeometry &GetGraphicsBackendGeometry() const
    {
        return m_GraphicsBackendGeometry;
    }

    inline const VertexAttributes &GetVertexAttributes() const
    {
        return m_VertexAttributes;
    }

protected:
    GraphicsBackendGeometry m_GraphicsBackendGeometry{};
    VertexAttributes m_VertexAttributes;

    DrawableGeometry(PrimitiveType primitiveType, int elementsCount, bool hasIndices);

private:
    PrimitiveType m_PrimitiveType;
    IndicesDataType m_IndicesDataType;
    int m_ElementsCount;
    bool m_HasIndices;
};

#endif