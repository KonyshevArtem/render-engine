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
                GraphicsBackendShaderObject shaders[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT];
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
                        shaders[shadersCount++] = CompileShader(shaderType, partPath, keywordsDirectives, SHADER_DIRECTIVES[i]);
                    }
                }
                else
                {
                    auto partPath = _path.parent_path() / passInfo.MetalShaderSourcePath;
                    shaders[0] = CompileShader(ShaderType::VERTEX_SHADER, partPath, keywordsDirectives, "");
                }

                auto program = GraphicsBackend::Current()->CreateProgram(&shaders[0], shadersCount, passInfo.ColorFormat, passInfo.DepthFormat);
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