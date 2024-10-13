#ifndef RENDER_ENGINE_SHADER_PARSER_H
#define RENDER_ENGINE_SHADER_PARSER_H

#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"

#include <string>
#include <unordered_map>

namespace ShaderParser
{
    void ParseReflection(const std::string& reflectionJson,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& textures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& buffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& samplers);
}

#endif //RENDER_ENGINE_SHADER_PARSER_H
