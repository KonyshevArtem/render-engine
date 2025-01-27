#ifndef RENDER_ENGINE_DX12_HELPERS_H
#define RENDER_ENGINE_DX12_HELPERS_H

#if RENDER_BACKEND_DX12

#include "enums/vertex_attribute_semantic.h"
#include "enums/vertex_attribute_data_type.h"

#include "d3d12.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"

namespace DX12Helpers
{
    const char* ToSemanticName(VertexAttributeSemantic semantic);
    DXGI_FORMAT ToVertexAttributeDataType(VertexAttributeDataType dataType, int dimensions, bool normalized);
}

#endif

#endif //RENDER_ENGINE_DX12_HELPERS_H
