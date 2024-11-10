#include "shader_loader.h"
#include "shader/shader.h"
#include "debug.h"
#include "graphics/graphics.h"
#include "file_system/file_system.h"
#include "enums/shader_type.h"
#include "types/graphics_backend_shader_object.h"
#include "graphics_backend_api.h"
#include "shader_parser.h"
#include "shader/shader_pass/shader_pass.h"

#include <span>

namespace ShaderLoader
{
    const int SUPPORTED_SHADERS_COUNT = 3;

    const std::string INSTANCING_KEYWORD = "_INSTANCING";

    const ShaderType SHADER_TYPES[SUPPORTED_SHADERS_COUNT]{
            ShaderType::VERTEX_SHADER,
            ShaderType::GEOMETRY_SHADER,
            ShaderType::FRAGMENT_SHADER};

    const std::string SHADER_SOURCE_FILE_NAME[SUPPORTED_SHADERS_COUNT]{
            "vs",
            "gs",
            "ps"
    };

    std::string GetBackendLiteral(GraphicsBackendName backendName)
    {
        switch (backendName)
        {
            case GraphicsBackendName::OPENGL:
                return "opengl";
            case GraphicsBackendName::METAL:
                return "metal";
            default:
                return "";
        }
    }

    std::string GetKeywordsHash(std::vector<std::string> keywords, bool& outSupportInstancing)
    {
        outSupportInstancing = false;

        std::string keywordsDirectives;
        std::sort(keywords.begin(), keywords.end());
        for (const auto &keyword: keywords)
        {
            keywordsDirectives += keyword + ",";
            outSupportInstancing |= keyword == INSTANCING_KEYWORD;
        }

        return std::to_string(std::hash<std::string>{}(keywordsDirectives));
    }

    std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords,
        BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo)
    {
        bool supportInstancing = false;
        std::string keywordHash = GetKeywordsHash(_keywords, supportInstancing);

        try
        {
            std::string backendLiteral = GetBackendLiteral(GraphicsBackend::Current()->GetName());
            std::string shaderName = _path.filename().replace_extension("").string();
            std::filesystem::path backendPath = FileSystem::GetResourcesPath() / _path.parent_path() / "output" / shaderName / backendLiteral / keywordHash;

            auto reflectionJson = FileSystem::ReadFile(backendPath / "reflection.json");
            std::unordered_map<std::string, GraphicsBackendTextureInfo> textures;
            std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers;
            std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers;
            ShaderParser::ParseReflection(reflectionJson, textures, buffers, samplers);

            std::vector<GraphicsBackendShaderObject> shaders;
            for (int i = 0; i < SUPPORTED_SHADERS_COUNT; ++i)
            {
                std::filesystem::path sourcePath = backendPath / SHADER_SOURCE_FILE_NAME[i];
                if (!std::filesystem::exists(sourcePath))
                    continue;

                std::string shaderSource = FileSystem::ReadFile(sourcePath);
                auto shader = GraphicsBackend::Current()->CompileShader(SHADER_TYPES[i], shaderSource);
                shaders.push_back(shader);
            }

            auto passPtr = std::make_shared<ShaderPass>(shaders, blendInfo, cullInfo, depthInfo, textures, buffers, samplers);

            std::vector<std::shared_ptr<ShaderPass>> passes;
            passes.push_back(passPtr);

            return std::make_shared<Shader>(passes, supportInstancing);
        }
        catch (const std::exception &_exception)
        {
            Debug::LogErrorFormat("[ShaderLoader] Can't load shader {}\n{}", _path.string(), _exception.what());
            return nullptr;
        }
    }
} // namespace ShaderLoader