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

#include <boost/json.hpp>
#include <span>
#include <unordered_map>

// TODO use syntax analysis instead of regex

namespace ShaderLoader
{
    constexpr int SUPPORTED_SHADERS_COUNT = 3;
    const std::string INSTANCING_KEYWORD = "_INSTANCING";

    const ShaderType SHADER_TYPES[SUPPORTED_SHADERS_COUNT] {
            ShaderType::VERTEX_SHADER,
            ShaderType::GEOMETRY_SHADER,
            ShaderType::FRAGMENT_SHADER};

    const std::string SHADER_NAMES[SUPPORTED_SHADERS_COUNT] {
            "vertex",
            "geometry",
            "fragment"};

    const std::string SHADER_DIRECTIVES[SUPPORTED_SHADERS_COUNT] {
            "#define VERTEX_PROGRAM\n",
            "#define GEOMETRY_PROGRAM\n",
            "#define FRAGMENT_PROGRAM\n"};

    struct ShaderPassInfo
    {
        Shader::PassInfo Pass;
        std::string ShaderPaths[SUPPORTED_SHADERS_COUNT];
    };

    struct ShaderInfo
    {
        std::vector<ShaderPassInfo>                  Passes;
        std::unordered_map<std::string, std::string> DefaultValues;
    };

    BlendFactor ParseBlendFactor(const std::string &_literal)
    {
        if (_literal == "SrcAlpha")
            return BlendFactor::SRC_ALPHA;

        if (_literal == "OneMinusSrcAlpha")
            return BlendFactor::ONE_MINUS_SRC_ALPHA;

        Debug::LogErrorFormat("[ShaderLoader] Unsupported blend factor: %1%", {_literal});
        return BlendFactor::ONE;
    }

    DepthFunction ParseDepthFunc(const std::string &_literal)
    {
        if (_literal == "Always")
            return DepthFunction::ALWAYS;

        Debug::LogErrorFormat("[Shader Loader] Unsupported depth func: %1%", {_literal});
        return DepthFunction::LEQUAL;
    }

    Shader::BlendInfo ParseBlendInfo(const boost::json::object &_obj)
    {
        using namespace boost::json;

        auto hasBlendInfo = _obj.contains("blend");

        Shader::BlendInfo info {hasBlendInfo};
        if (hasBlendInfo)
        {
            auto &infoObj = _obj.at("blend").as_object();
            info.SourceFactor = ParseBlendFactor(value_to<std::string>(infoObj.at("SrcFactor")));
            info.DestinationFactor = ParseBlendFactor(value_to<std::string>(infoObj.at("DstFactor")));
        }

        return info;
    }

    Shader::CullInfo ParseCullInfo(const boost::json::object &_obj)
    {
        using namespace boost::json;

        Shader::CullInfo info {true, CullFace::BACK};
        if (_obj.contains("cull"))
        {
            auto cull = value_to<std::string>(_obj.at("cull"));

            if (cull == "Front")
                info.Face = CullFace::FRONT;
            else if (cull == "Back")
                info.Face = CullFace::BACK;
            else if (cull == "FrontBack")
                info.Face = CullFace::FRONT_AND_BACK;
            else if (cull == "None")
                info.Enabled = false;
        }

        return info;
    }

    Shader::DepthInfo ParseDepthInfo(const boost::json::object &_obj)
    {
        using namespace boost::json;

        Shader::DepthInfo info;

        info.WriteDepth = !_obj.contains("zWrite") || _obj.at("zWrite").as_bool();
        if (_obj.contains("zTest"))
            info.DepthFunction = ParseDepthFunc(value_to<std::string>(_obj.at("zTest")));

        return info;
    }

    Shader::PassInfo ParsePassInfo(const boost::json::object &_obj)
    {
        using namespace boost::json;

        Shader::PassInfo info;

        if (_obj.contains("tags"))
            info.Tags = value_to<std::unordered_map<std::string, std::string>>(_obj.at("tags"));

        info.DepthInfo = ParseDepthInfo(_obj);
        info.BlendInfo = ParseBlendInfo(_obj);
        info.CullInfo = ParseCullInfo(_obj);
        return info;
    }

    ShaderPassInfo tag_invoke(const boost::json::value_to_tag<ShaderPassInfo> &, boost::json::value const &_value)
    {
        using namespace boost::json;

        auto &obj = _value.as_object();

        ShaderPassInfo info {ParsePassInfo(obj)};

        for (int i = 0; i < SUPPORTED_SHADERS_COUNT; ++i)
        {
            if (obj.contains(SHADER_NAMES[i]))
                info.ShaderPaths[i] = value_to<std::string>(obj.at(SHADER_NAMES[i]));
        }

        return info;
    }

    ShaderInfo tag_invoke(const boost::json::value_to_tag<ShaderInfo> &, boost::json::value const &_value)
    {
        using namespace boost::json;

        auto &obj = _value.as_object();

        ShaderInfo info {value_to<std::vector<ShaderPassInfo>>(obj.at("passes"))};

        // parse default values
        if (obj.contains("properties"))
            info.DefaultValues = value_to<std::unordered_map<std::string, std::string>>(obj.at("properties"));

        return info;
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

        auto shader = GraphicsBackend::CreateShader(shaderType);
        GraphicsBackend::SetShaderSources(shader, sourcesCount, sources, nullptr);

        GraphicsBackend::CompileShader(shader);

        int isCompiled;
        GraphicsBackend::GetShaderParameter(shader, ShaderParameter::COMPILE_STATUS, &isCompiled);
        if (!isCompiled)
        {
            int infoLogLength;
            GraphicsBackend::GetShaderParameter(shader, ShaderParameter::INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            GraphicsBackend::GetShaderInfoLog(shader, infoLogLength, nullptr, &logMsg[0]);

            throw std::runtime_error("Compilation failed with errors:\n" + logMsg);
        }

        return shader;
    }

    GraphicsBackendProgram LinkProgram(const std::span<GraphicsBackendShaderObject> &shaders)
    {
        auto program = GraphicsBackend::CreateProgram();

        for (const auto &shader: shaders)
        {
            if (GraphicsBackend::IsShader(shader))
                GraphicsBackend::AttachShader(program, shader);
        }

        GraphicsBackend::LinkProgram(program);

        for (const auto &shader: shaders)
        {
            if (GraphicsBackend::IsShader(shader))
            {
                GraphicsBackend::DetachShader(program, shader);
                GraphicsBackend::DeleteShader(shader);
            }
        }

        int isLinked;
        GraphicsBackend::GetProgramParameter(program, ProgramParameter::LINK_STATUS, &isLinked);
        if (!isLinked)
        {
            int infoLogLength;
            GraphicsBackend::GetProgramParameter(program, ProgramParameter::INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            GraphicsBackend::GetProgramInfoLog(program, infoLogLength, nullptr, &logMsg[0]);

            throw std::runtime_error("Link failed with error: " + logMsg);
        }

        return program;
    }

    ShaderInfo ParseShaderInfo(const std::string &_shaderSource)
    {
        auto value = boost::json::parse(_shaderSource);
        return boost::json::value_to<ShaderInfo>(value);
    }

    std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords)
    {
        auto        shaderSource      = Utils::ReadFileWithIncludes(_path);
        bool        supportInstancing = false;
        std::string keywordsDirectives;
        for (const auto &keyword: _keywords)
        {
            keywordsDirectives += "#define " + keyword + "\n";
            supportInstancing |= keyword == INSTANCING_KEYWORD;
        }

        try
        {
            ShaderInfo shaderInfo = ParseShaderInfo(shaderSource);

            std::vector<Shader::PassInfo> passes;
            for (auto &passInfo: shaderInfo.Passes)
            {
                GraphicsBackendShaderObject shaders[SUPPORTED_SHADERS_COUNT];
                size_t shadersCount = 0;

                for (int i = 0; i < SUPPORTED_SHADERS_COUNT; ++i)
                {
                    auto shaderType = SHADER_TYPES[i];
                    auto &relativePath = passInfo.ShaderPaths[i];
                    if (relativePath.empty())
                        continue;

                    auto partPath = _path.parent_path() / relativePath;
                    auto partSource = Utils::ReadFileWithIncludes(partPath);
                    shaders[shadersCount++] = CompileShader(shaderType, partSource, keywordsDirectives, SHADER_DIRECTIVES[i]);
                }

                passInfo.Pass.Program = LinkProgram(std::span<GraphicsBackendShaderObject> {shaders, shadersCount});

                passes.push_back(passInfo.Pass);
            }

            return std::shared_ptr<Shader>(new Shader(passes, shaderInfo.DefaultValues, supportInstancing));
        }
        catch (const std::exception &_exception)
        {
            Debug::LogErrorFormat("[ShaderLoader] Can't load shader %1%\n%2%", {_path.string(), _exception.what()});
            return nullptr;
        }
    }

#pragma endregion

} // namespace ShaderLoader