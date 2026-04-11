#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_DESCRIPTOR_H

#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_vertex_attribute_descriptor.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_stencil_descriptor.h"
#include "types/graphics_backend_depth_descriptor.h"
#include "types/graphics_backend_rasterizer_descriptor.h"
#include "types/graphics_backend_tlas_info.h"
#include "enums/texture_internal_format.h"
#include "enums/primitive_type.h"
#include "enums/program_type.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

struct ThreadGroupSize
{
    uint32_t X = 1;
    uint32_t Y = 1;
    uint32_t Z = 1;
};

struct GraphicsBackendProgramDescriptor
{
    ProgramType Type;
    const std::vector<GraphicsBackendShaderObject>* Shaders;
    const std::vector<GraphicsBackendVertexAttributeDescriptor>* VertexAttributes;
    const std::unordered_map<std::string, GraphicsBackendTextureInfo>* Textures;
    const std::unordered_map<std::string, GraphicsBackendSamplerInfo>* Samplers;
    const std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>* Buffers;
    const std::unordered_map<std::string, GraphicsBackendTLASInfo>* TLASes;
    const std::string* Name;

    GraphicsBackendColorAttachmentDescriptor ColorAttachmentDescriptor;
    TextureInternalFormat DepthFormat;

    GraphicsBackendRasterizerDescriptor RasterizerDescriptor;
    GraphicsBackendDepthDescriptor DepthDescriptor;
    GraphicsBackendStencilDescriptor StencilDescriptor;

    PrimitiveType PrimitiveType;

    ThreadGroupSize ThreadGroupSize;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_DESCRIPTOR_H
