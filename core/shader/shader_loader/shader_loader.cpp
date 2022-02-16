#include "shader_loader.h"
#include "utils.h"
#include "core_debug/debug.h"
#include "graphics/graphics.h"
#include "../shader.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <boost/algorithm/string.hpp>
#include <regex>
#include <span>
#include <unordered_map>
#include <iostream>

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
        Shader::BlendInfo                            BlendInfo;
    };

    bool TryCompileShaderPart(GLuint             _shaderPartType,
                              const std::string &_path,
                              const std::string &_source,
                              const std::string &_keywordDirectives,
                              GLuint &           _outShaderPart)
    {
        const auto &  globalDirectives = Graphics::GetGlobalShaderDirectives();
        constexpr int sourcesCount     = 3;
        const char *  sources[sourcesCount];

        sources[0] = globalDirectives.c_str();
        sources[1] = _keywordDirectives.c_str();
        sources[2] = _source.c_str();

        _outShaderPart = glCreateShader(_shaderPartType);
        glShaderSource(_outShaderPart, sourcesCount, sources, nullptr);

        glCompileShader(_outShaderPart);

        GLint status;
        glGetShaderiv(_outShaderPart, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(_outShaderPart, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            glGetShaderInfoLog(_outShaderPart, infoLogLength, nullptr, &logMsg[0]);

            Debug::LogErrorFormat("[ShaderLoader] Compilation failed: %1%\n%2%", {_path, logMsg});

            return false;
        }

        return true;
    }

    bool TryLinkProgram(const std::span<GLuint> &_shaderParts, GLuint &_outProgram, const std::string &_path)
    {
        _outProgram = glCreateProgram();

        for (const auto &part: _shaderParts)
        {
            if (glIsShader(part))
                glAttachShader(_outProgram, part);
        }

        glLinkProgram(_outProgram);

        for (const auto &part: _shaderParts)
        {
            if (glIsShader(part))
            {
                glDetachShader(_outProgram, part);
                glDeleteShader(part);
            }
        }

        GLint status;
        glGetProgramiv(_outProgram, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetProgramiv(_outProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::string logMsg(infoLogLength + 1, ' ');
            glGetProgramInfoLog(_outProgram, infoLogLength, nullptr, &logMsg[0]);

            Debug::LogErrorFormat("[ShaderLoader] Link failed: %1%\n%2%", {_path, logMsg});

            return false;
        }

        return true;
    }

    GLenum ParseBlendFunc(const std::string &_literal)
    {
        if (_literal == "SrcAlpha")
            return GL_SRC_ALPHA;

        if (_literal == "OneMinusSrcAlpha")
            return GL_ONE_MINUS_SRC_ALPHA;

        Debug::LogErrorFormat("[ShaderLoader] Unsupported blend func: %1%", {_literal});
        return GL_ONE;
    }

    ShaderInfo ParseShaderInfo(const std::string &_shaderSource)
    {
        ShaderInfo                  shaderInfo;
        std::smatch                 match;
        std::regex                  expression(R"(\s*#pragma\s+(.+)\n)");
        std::string::const_iterator searchStart(_shaderSource.cbegin());

        while (regex_search(searchStart, _shaderSource.cend(), match, expression))
        {
            auto                     info = match[1].str();
            std::vector<std::string> strings;
            boost::split(strings, info, boost::is_any_of(" "));
            unsigned long length = strings.size();

            // parse default values
            if (strings[0] == "default")
            {
                if (length < 3)
                {
                    Debug::LogErrorFormat("[ShaderLoader] Default value info has incorrect format: %1%", {info});
                    continue;
                }

                shaderInfo.DefaultValues[strings[1]] = strings[2];
            }

            // parse tags
            if (strings[0] == "tag")
            {
                if (length < 3)
                {
                    Debug::LogErrorFormat("[ShaderLoader] Tag has incorrect format: %1%", {info});
                    continue;
                }

                shaderInfo.Tags[strings[1]] = strings[2];
            }

            // parse writing to depth
            if (strings[0] == "zWrite")
            {
                if (length < 2)
                {
                    Debug::LogErrorFormat("[ShaderLoader] ZWrite has incorrect format: %1%", {info});
                    continue;
                }

                if (strings[1] == "off")
                    shaderInfo.ZWrite = GL_FALSE;
                else if (strings[1] == "on")
                    shaderInfo.ZWrite = GL_TRUE;
                else
                    Debug::LogErrorFormat("[ShaderLoader] ZWrite has incorrect format: %1%", {info});
            }

            // parse blend mode
            if (strings[0] == "blend")
            {
                if (length < 3)
                {
                    Debug::LogErrorFormat("[ShaderLoader] Blend info has incorrect format: %1%", {info});
                    continue;
                }

                shaderInfo.BlendInfo.Enabled   = true;
                shaderInfo.BlendInfo.SrcFactor = ParseBlendFunc(strings[1]);
                shaderInfo.BlendInfo.DstFactor = ParseBlendFunc(strings[2]);
            }

            // parse paths to shader parts
            if (length < 2)
                continue;

            for (int i = 0; i < SHADER_PART_COUNT; ++i)
            {
                if (strings[0] == SHADER_PART_NAMES[i])
                    shaderInfo.ShaderPartPaths[i] = strings[1];
            }

            searchStart = match.suffix().first;
        }

        return shaderInfo;
    }

    std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords)
    {
        auto        shaderSource = Utils::ReadFileWithIncludes(_path);
        std::string keywordsDirectives;
        for (const auto &keyword: _keywords)
            keywordsDirectives += "#define " + keyword + "\n";

        ShaderInfo shaderInfo = ParseShaderInfo(shaderSource);

        bool   success = true;
        GLuint shaderParts[SHADER_PART_COUNT];
        size_t shaderPartCount = 0;

        for (int i = 0; i < SHADER_PART_COUNT; ++i)
        {
            auto        part         = SHADER_PARTS[i];
            const auto &partName     = SHADER_PART_NAMES[i];
            auto &      relativePath = shaderInfo.ShaderPartPaths[i];
            if (relativePath.length() == 0)
                continue;

            GLuint shaderPart = 0;
            auto   partPath   = _path.parent_path() / relativePath;
            auto   partSource = Utils::ReadFileWithIncludes(partPath);
            success &= TryCompileShaderPart(part, partPath.string(), partSource, keywordsDirectives, shaderPart);

            shaderParts[shaderPartCount++] = shaderPart;
        }

        GLuint program;
        success &= TryLinkProgram(std::span<GLuint> {shaderParts, shaderPartCount}, program, _path.string());

        return success ? std::shared_ptr<Shader>(
                                 new Shader(program,
                                            shaderInfo.DefaultValues,
                                            shaderInfo.Tags,
                                            shaderInfo.ZWrite,
                                            shaderInfo.BlendInfo))
                       : nullptr;
    }

#pragma endregion

} // namespace ShaderLoader