#include "drawable_geometry.h"
#include "graphics_backend_api.h"

DrawableGeometry::DrawableGeometry(PrimitiveType primitiveType, int elementsCount, bool hasIndices) :
    m_PrimitiveType(primitiveType),
    m_IndicesDataType(IndicesDataType::UNSIGNED_INT),
    m_ElementsCount(elementsCount),
    m_HasIndices(hasIndices)
{
}

DrawableGeometry::~DrawableGeometry()
{
    GraphicsBackend::Current()->DeleteGeometry(m_GraphicsBackendGeometry);
}