#include "shader_loader.h"
#include "shader/shader.h"
#include "debug.h"
#include "graphics/graphics.h"
#include "utils.h"
#include "enums/shader_type.h"
#include "enums/shader_parameter.h"
#include "enums/program_parameter.h"
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

    std::string GetShaderTypeName(ShaderType shaderType)
    {
        switch (shaderType)
        {
            case ShaderType::VERTEX_SHADER:
                return "Vertex";
            case ShaderType::FRAGMENT_SHADER:
                return "Fragment";
            case ShaderType::GEOMETRY_SHADER:
                return "Geometry";
            default:
                return "Unknown";
        }
    }

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source, const std::string &keywordDirectives, const std::string &shaderPartDirective)
    {
        constexpr int sourcesCount = 4;

        const auto &globalDirectives = Graphics::GetGlobalShaderDirectives();
        const char *sources[sourcesCount]{
                globalDirectives.c_str(),
                keywordDirectives.c_str(),
                shaderPartDirective.c_str(),
                source.c_str()};

        auto shader = GraphicsBackend::Current()->CreateShader(shaderType);
        GraphicsBackend::Current()->SetShaderSources(shader, sourcesCount, sources, nullptr);

        GraphicsBackend::Current()->CompileShader(shader);

        int isCompiled;
        GraphicsBackend::Current()->GetShaderParameter(shader, ShaderParameter::COMPILE_STATUS, &isCompiled);
        if (!isCompiled)
        {
            int infoLogLength;
            GraphicsBackend::Current()->GetShaderParameter(shader, ShaderParameter::INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            GraphicsBackend::Current()->GetShaderInfoLog(shader, infoLogLength, nullptr, &logMsg[0]);

            throw std::runtime_error(GetShaderTypeName(shaderType) + " shader compilation failed with errors:\n" + logMsg);
        }

        return shader;
    }

    GraphicsBackendProgram LinkProgram(const std::span<GraphicsBackendShaderObject> &shaders)
    {
        auto program = GraphicsBackend::Current()->CreateProgram();

        for (const auto &shader: shaders)
        {
            if (GraphicsBackend::Current()->IsShader(shader))
                GraphicsBackend::Current()->AttachShader(program, shader);
        }

        GraphicsBackend::Current()->LinkProgram(program);

        for (const auto &shader: shaders)
        {
            if (GraphicsBackend::Current()->IsShader(shader))
            {
                GraphicsBackend::Current()->DetachShader(program, shader);
                GraphicsBackend::Current()->DeleteShader(shader);
            }
        }

        int isLinked;
        GraphicsBackend::Current()->GetProgramParameter(program, ProgramParameter::LINK_STATUS, &isLinked);
        if (!isLinked)
        {
            int infoLogLength;
            GraphicsBackend::Current()->GetProgramParameter(program, ProgramParameter::INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            GraphicsBackend::Current()->GetProgramInfoLog(program, infoLogLength, nullptr, &logMsg[0]);

            throw std::runtime_error("Link failed with error:\n" + logMsg);
        }

        return program;
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
                GraphicsBackendShaderObject shaders[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT];
                size_t shadersCount = 0;

                for (int i = 0; i < ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT; ++i)
                {
                    auto shaderType = SHADER_TYPES[i];
                    auto &relativePath = passInfo.ShaderPaths[i];
                    if (relativePath.empty())
                        continue;

                    auto partPath = _path.parent_path() / relativePath;
                    auto partSource = Utils::ReadFileWithIncludes(partPath);
                    shaders[shadersCount++] = CompileShader(shaderType, partSource, keywordsDirectives, SHADER_DIRECTIVES[i]);
                }

                auto program = LinkProgram(std::span<GraphicsBackendShaderObject>{shaders, shadersCount});
                auto passPtr = std::make_shared<ShaderPass>(program, passInfo.BlendInfo, passInfo.CullInfo, passInfo.DepthInfo, passInfo.Tags, properties);
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

#pragma endregion

} // namespace ShaderLoader