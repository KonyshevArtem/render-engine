#ifndef RENDER_ENGINE_SHADER_PARSER_H
#define RENDER_ENGINE_SHADER_PARSER_H

#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_program_descriptor.h"
#include "types/graphics_backend_tlas_info.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace ShaderParser
{
    void ParseReflection(const std::string& reflectionJson,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& textures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& buffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& samplers,
        std::unordered_map<std::string, GraphicsBackendTLASInfo>& TLASes,
        ThreadGroupSize& threadGroupSize);
}

#endif //RENDER_ENGINE_SHADER_PARSER_H
