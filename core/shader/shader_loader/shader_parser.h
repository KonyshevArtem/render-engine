#ifndef RENDER_ENGINE_SHADER_PARSER_H
#define RENDER_ENGINE_SHADER_PARSER_H

#include "shader/shader_structs.h"
#include "shader_loader_utils.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace ShaderParser
{
    struct PassInfo
    {
        BlendInfo BlendInfo;
        CullInfo CullInfo;
        DepthInfo DepthInfo;

        std::string OpenGLShaderSourcePaths[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT];
        std::string MetalShaderSourcePath;
    };

    void Parse(const std::string &shaderSource, std::vector<ShaderParser::PassInfo> &passes, std::unordered_map<std::string, std::string> &properties);
    void ParseReflection(const std::string& reflectionJson,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& textures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& buffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& samplers);
}

#endif //RENDER_ENGINE_SHADER_PARSER_H
