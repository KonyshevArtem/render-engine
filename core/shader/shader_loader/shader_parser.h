#ifndef RENDER_ENGINE_SHADER_PARSER_H
#define RENDER_ENGINE_SHADER_PARSER_H

#include "shader/shader_structs.h"
#include "shader_loader_utils.h"
#include "enums/texture_internal_format.h"

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
        TextureInternalFormat ColorFormat;
        TextureInternalFormat DepthFormat;

        std::unordered_map<std::string, std::string> Tags;
        std::string OpenGLShaderSourcePaths[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT];
        std::string MetalShaderSourcePath;
    };

    void Parse(const std::string &shaderSource, std::vector<ShaderParser::PassInfo> &passes, std::unordered_map<std::string, std::string> &properties);
}

#endif //RENDER_ENGINE_SHADER_PARSER_H
