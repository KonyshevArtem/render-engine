#ifndef RENDER_ENGINE_DRAWABLE_GEOMETRY
#define RENDER_ENGINE_DRAWABLE_GEOMETRY

#include "enums/primitive_type.h"
#include "types/graphics_backend_geometry.h"
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

    inline const GraphicsBackendGeometry &GetGraphicsBackendGeometry() const
    {
        return m_GraphicsBackendGeometry;
    }

protected:
    GraphicsBackendGeometry m_GraphicsBackendGeometry{};

    DrawableGeometry(PrimitiveType primitiveType, int elementsCount, bool hasIndices);

private:
    PrimitiveType m_PrimitiveType;
    int m_ElementsCount;
    bool m_HasIndices;
};

#endif