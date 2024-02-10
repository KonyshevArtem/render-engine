#ifndef OPENGL_STUDY_SHADER_PARSER_H
#define OPENGL_STUDY_SHADER_PARSER_H

#include "shader/shader_structs.h"
#include "shader_loader_utils.h"

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

        std::unordered_map<std::string, std::string> Tags;
        std::string ShaderPaths[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT];
    };

    void Parse(const std::string &shaderSource, std::vector<ShaderParser::PassInfo> &passes, std::unordered_map<std::string, std::string> &properties);
}

#endif //OPENGL_STUDY_SHADER_PARSER_H
