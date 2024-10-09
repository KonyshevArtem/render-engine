#include "shader_loader.h"
#include "shader/shader.h"
#include "debug.h"
#include "graphics/graphics.h"
#include "utils.h"
#include "enums/shader_type.h"
#include "types/graphics_backend_shader_object.h"
#include "graphics_backend_api.h"
#include "shader_parser.h"
#include "shader_loader_utils.h"
#include "shader/shader_pass/shader_pass.h"

#include <span>

namespace ShaderLoader
{
    const std::string INSTANCING_KEYWORD = "_INSTANCING";

    const ShaderType SHADER_TYPES[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT]{
            ShaderType::VERTEX_SHADER,
            ShaderType::GEOMETRY_SHADER,
            ShaderType::FRAGMENT_SHADER};

    const std::string SHADER_DIRECTIVES[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT]{
            "#define VERTEX_PROGRAM\n",
            "#define GEOMETRY_PROGRAM\n",
            "#define FRAGMENT_PROGRAM\n"};

    const std::string SHADER_SOURCE_FILE_NAME[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT]{
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

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::filesystem::path &partPath, const std::string &keywordDirectives, const std::string &shaderPartDirective)
    {
        auto source = Utils::ReadFileWithIncludes(partPath);
        auto &globalDirectives = Graphics::GetGlobalShaderDirectives();
        auto fullSource = globalDirectives + "\n" + keywordDirectives + "\n" + shaderPartDirective + "\n" + source;
        return GraphicsBackend::Current()->CompileShader(shaderType, fullSource);
    }

    std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords)
    {
        bool supportInstancing = false;
        std::string keywordsDirectives;
        for (const auto &keyword: _keywords)
        {
            keywordsDirectives += "#define " + keyword + "\n";
            supportInstancing |= keyword == INSTANCING_KEYWORD;
        }

        try
        {
            auto shaderSource = Utils::ReadFileWithIncludes(_path);

            std::vector<ShaderParser::PassInfo> passesInfo;
            std::unordered_map<std::string, std::string> properties;
            ShaderParser::Parse(shaderSource, passesInfo, properties);

            std::vector<std::shared_ptr<ShaderPass>> passes;
            for (auto &passInfo: passesInfo)
            {
                std::vector<GraphicsBackendShaderObject> shaders;
                int shadersCount = 0;

                if (GraphicsBackend::Current()->GetName() == GraphicsBackendName::OPENGL)
                {
                    for (int i = 0; i < ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT; ++i)
                    {
                        auto shaderType = SHADER_TYPES[i];
                        auto &relativePath = passInfo.OpenGLShaderSourcePaths[i];
                        if (relativePath.empty())
                            continue;

                        auto partPath = _path.parent_path() / relativePath;
                        shaders.push_back(CompileShader(shaderType, partPath, keywordsDirectives, SHADER_DIRECTIVES[i]));
                    }
                }
                else
                {
                    auto partPath = _path.parent_path() / passInfo.MetalShaderSourcePath;
                    shaders.push_back(CompileShader(ShaderType::VERTEX_SHADER, partPath, keywordsDirectives, ""));
                }

                auto passPtr = std::make_shared<ShaderPass>(shaders, passInfo.BlendInfo, passInfo.CullInfo, passInfo.DepthInfo, passInfo.Tags, properties);
                passes.push_back(passPtr);
            }

            return std::make_shared<Shader>(passes, supportInstancing);
        }
        catch (const std::exception &_exception)
        {
            Debug::LogErrorFormat("[ShaderLoader] Can't load shader %1%\n%2%", {_path.string(), _exception.what()});
            return nullptr;
        }
    }

    std::shared_ptr<Shader> Load2(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords,
        BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo, std::unordered_map<std::string, std::string> tags)
    {
        bool supportInstancing = false;
        std::string keywordHash = GetKeywordsHash(_keywords, supportInstancing);

        try
        {
            std::string backendLiteral = GetBackendLiteral(GraphicsBackend::Current()->GetName());
            std::filesystem::path backendPath = Utils::GetExecutableDirectory() / _path.parent_path() / "output" / backendLiteral / keywordHash;

            auto reflectionJson = Utils::ReadFile(backendPath / "reflection.json");
            std::unordered_map<std::string, GraphicsBackendTextureInfo> textures;
            std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers;
            std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers;
            ShaderParser::ParseReflection(reflectionJson, textures, buffers, samplers);

            std::vector<GraphicsBackendShaderObject> shaders;
            for (int i = 0; i < ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT; ++i)
            {
                std::filesystem::path sourcePath = backendPath / SHADER_SOURCE_FILE_NAME[i];
                if (!std::filesystem::exists(sourcePath))
                    continue;

                std::string shaderSource = Utils::ReadFile(sourcePath);
                auto shader = GraphicsBackend::Current()->CompileShader(SHADER_TYPES[i], shaderSource);
                shaders.push_back(shader);
            }

            std::unordered_map<std::string, std::string> properties;
            auto passPtr = std::make_shared<ShaderPass>(shaders, blendInfo, cullInfo, depthInfo, tags, properties, textures, buffers, samplers);

            std::vector<std::shared_ptr<ShaderPass>> passes;
            passes.push_back(passPtr);

            return std::make_shared<Shader>(passes, supportInstancing);
        }
        catch (const std::exception &_exception)
        {
            Debug::LogErrorFormat("[ShaderLoader] Can't load shader %1%\n%2%", {_path.string(), _exception.what()});
            return nullptr;
        }
    }
} // namespace ShaderLoader