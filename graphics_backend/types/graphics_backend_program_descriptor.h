#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_DESCRIPTOR_H

#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_vertex_attribute_descriptor.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"
#include "enums/texture_internal_format.h"
#include "enums/cull_face.h"
#include "enums/cull_face_orientation.h"
#include "enums/primitive_type.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

struct GraphicsBackendProgramDescriptor
{
    const std::vector<GraphicsBackendShaderObject>* Shaders;
    const std::vector<GraphicsBackendVertexAttributeDescriptor>* VertexAttributes;
    const std::unordered_map<std::string, GraphicsBackendTextureInfo>* Textures;
    const std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>* Buffers;
    const std::unordered_map<std::string, GraphicsBackendSamplerInfo>* Samplers;
    const std::string* Name;

    GraphicsBackendColorAttachmentDescriptor ColorAttachmentDescriptor;
    TextureInternalFormat DepthFormat;

    CullFace CullFace;
    CullFaceOrientation CullFaceOrientation;

    bool DepthWrite;
    DepthFunction DepthFunction;

    PrimitiveType PrimitiveType;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_DESCRIPTOR_H
