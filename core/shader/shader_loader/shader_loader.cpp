#include "shader_loader.h"
#include "../shader.h"
#include "core_debug/debug.h"
#include "graphics/graphics.h"
#include "utils.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <boost/json.hpp>
#include <span>
#include <unordered_map>

// TODO use syntax analysis instead of regex

namespace ShaderLoader
{
    constexpr int SHADER_PART_COUNT = 3;

    const GLuint SHADER_PARTS[SHADER_PART_COUNT] {
            GL_VERTEX_SHADER,
            GL_GEOMETRY_SHADER,
            GL_FRAGMENT_SHADER};

    const std::string SHADER_PART_NAMES[SHADER_PART_COUNT] {
            "vertex",
            "geometry",
            "fragment"};

    struct ShaderInfo
    {
        std::unordered_map<std::string, std::string> DefaultValues;
        std::unordered_map<std::string, std::string> Tags;
        std::string                                  ShaderPartPaths[SHADER_PART_COUNT];
        bool                                         ZWrite = true;
        GLenum                                       ZTest  = GL_LEQUAL;
        Shader::BlendInfo                            BlendInfo;
    };

    GLenum ParseBlendFunc(const std::string &_literal)
    {
        if (_literal == "SrcAlpha")
            return GL_SRC_ALPHA;

        if (_literal == "OneMinusSrcAlpha")
            return GL_ONE_MINUS_SRC_ALPHA;

        Debug::LogErrorFormat("[ShaderLoader] Unsupported blend func: %1%", {_literal});
        return GL_ONE;
    }

    GLenum ParseDepthFunc(const std::string &_literal)
    {
        if (_literal == "Always")
            return GL_ALWAYS;

        Debug::LogErrorFormat("[Shader Loader] Unsupported depth func: %1%", {_literal});
        return GL_LEQUAL;
    }

    Shader::BlendInfo ParseBlendInfo(const boost::json::object &_obj)
    {
        using namespace boost::json;

        auto hasBlendInfo = _obj.contains("blend");

        Shader::BlendInfo info {hasBlendInfo};
        if (hasBlendInfo)
        {
            auto &infoObj  = _obj.at("blend").as_object();
            info.SrcFactor = ParseBlendFunc(value_to<std::string>(infoObj.at("SrcFactor")));
            info.DstFactor = ParseBlendFunc(value_to<std::string>(infoObj.at("DstFactor")));
        }

        return info;
    }

    ShaderInfo tag_invoke(const boost::json::value_to_tag<ShaderInfo> &, boost::json::value const &_value)
    {
        using namespace boost::json;

        auto &obj = _value.as_object();

        ShaderInfo info;

        // parse default values
        if (obj.contains("default"))
            info.DefaultValues = value_to<std::unordered_map<std::string, std::string>>(obj.at("default"));

        // parse tags
        if (obj.contains("tags"))
            info.Tags = value_to<std::unordered_map<std::string, std::string>>(obj.at("tags"));

        // parse zWrite
        info.ZWrite = !obj.contains("zWrite") || obj.at("zWrite").as_bool();

        // parse z func
        if (obj.contains("zTest"))
            info.ZTest = ParseDepthFunc(value_to<std::string>(obj.at("zTest")));

        // parse blend
        info.BlendInfo = ParseBlendInfo(obj);

        // parse paths to shader code
        for (int i = 0; i < SHADER_PART_COUNT; ++i)
        {
            if (obj.contains(SHADER_PART_NAMES[i]))
                info.ShaderPartPaths[i] = value_to<std::string>(obj.at(SHADER_PART_NAMES[i]));
        }

        return info;
    }

    GLuint CompileShaderPart(GLuint _shaderPartType, const std::string &_source, const std::string &_keywordDirectives)
    {
        const auto   &globalDirectives = Graphics::GetGlobalShaderDirectives();
        constexpr int sourcesCount     = 3;
        const char   *sources[sourcesCount] {
                globalDirectives.c_str(),
                _keywordDirectives.c_str(),
                _source.c_str()};

        auto shaderPart = glCreateShader(_shaderPartType);
        glShaderSource(shaderPart, sourcesCount, sources, nullptr);

        glCompileShader(shaderPart);

        GLint status;
        glGetShaderiv(shaderPart, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(shaderPart, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            glGetShaderInfoLog(shaderPart, infoLogLength, nullptr, &logMsg[0]);

            throw std::runtime_error("Compilation failed with error: " + logMsg);
        }

        return shaderPart;
    }

    GLuint LinkProgram(const std::span<GLuint> &_shaderParts)
    {
        auto program = glCreateProgram();

        for (const auto &part: _shaderParts)
        {
            if (glIsShader(part))
                glAttachShader(program, part);
        }

        glLinkProgram(program);

        for (const auto &part: _shaderParts)
        {
            if (glIsShader(part))
            {
                glDetachShader(program, part);
                glDeleteShader(part);
            }
        }

        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            glGetProgramInfoLog(program, infoLogLength, nullptr, &logMsg[0]);

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
        auto        shaderSource = Utils::ReadFileWithIncludes(_path);
        std::string keywordsDirectives;
        for (const auto &keyword: _keywords)
            keywordsDirectives += "#define " + keyword + "\n";

        try
        {
            ShaderInfo shaderInfo = ParseShaderInfo(shaderSource);
            GLuint     shaderParts[SHADER_PART_COUNT];
            size_t     shaderPartCount = 0;

            for (int i = 0; i < SHADER_PART_COUNT; ++i)
            {
                auto  part         = SHADER_PARTS[i];
                auto &relativePath = shaderInfo.ShaderPartPaths[i];
                if (relativePath.empty())
                    continue;

                auto partPath                  = _path.parent_path() / relativePath;
                auto partSource                = Utils::ReadFileWithIncludes(partPath);
                shaderParts[shaderPartCount++] = CompileShaderPart(part, partSource, keywordsDirectives);
            }

            auto program = LinkProgram(std::span<GLuint> {shaderParts, shaderPartCount});

            return std::shared_ptr<Shader>(new Shader(program,
                                                      shaderInfo.DefaultValues,
                                                      shaderInfo.Tags,
                                                      shaderInfo.ZWrite,
                                                      shaderInfo.ZTest,
                                                      shaderInfo.BlendInfo));
        }
        catch (std::exception _exception)
        {
            Debug::LogErrorFormat("[ShaderLoader] Can't load shader %1%\n%2%", {_path.string(), _exception.what()});
            return nullptr;
        }
    }

#pragma endregion

} // namespace ShaderLoader