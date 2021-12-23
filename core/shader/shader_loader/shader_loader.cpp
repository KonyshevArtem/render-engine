#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "shader_loader.h"
#include "../../../utils/utils.h"
#include "../../graphics/graphics.h"
#include "../shader.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <regex>

// TODO use syntax analysis instead of regex

namespace ShaderLoader
{
    constexpr int SHADER_PART_COUNT = 3;

    const GLuint SHADER_PARTS[SHADER_PART_COUNT] {
            GL_VERTEX_SHADER,
            GL_GEOMETRY_SHADER,
            GL_FRAGMENT_SHADER};

    const string SHADER_PART_NAMES[SHADER_PART_COUNT] {
            "vertex",
            "geometry",
            "fragment"};

    bool TryCompileShaderPart(GLuint        _shaderPartType,
                              const string &_path,
                              const string &_source,
                              const string &_keywordDirectives,
                              GLuint &      _outShaderPart)
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

            GLchar logMsg[infoLogLength + 1];
            glGetShaderInfoLog(_outShaderPart, infoLogLength, nullptr, logMsg);

            fprintf(stderr, "Shader compilation failed: %s\n%s\n", _path.c_str(), logMsg);

            return false;
        }

        return true;
    }

    bool TryLinkProgram(const span<GLuint> &_shaderParts, GLuint &_outProgram, const string &_path)
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

            GLchar logMsg[infoLogLength + 1];
            glGetProgramInfoLog(_outProgram, infoLogLength, nullptr, logMsg);
            fprintf(stderr, "Program link failed: %s\n%s", _path.c_str(), logMsg);

            return false;
        }

        return true;
    }

    void ParsePragmas(const string &                 _shaderSource,
                      unordered_map<string, string> &_defaultValues,
                      unordered_map<string, string> &_additionalParameters)
    {
        smatch                 match;
        regex                  expression(R"(\s*#pragma\s+(.+)\n)");
        string::const_iterator searchStart(_shaderSource.cbegin());

        while (regex_search(searchStart, _shaderSource.cend(), match, expression))
        {
            auto           info = match[1].str();
            vector<string> strings;
            boost::split(strings, info, boost::is_any_of(" "));
            unsigned long length = strings.size();

            if (strings[0] == "default")
            {
                if (length < 3)
                {
                    fprintf(stderr, "Default value info has incorrect format: %s\n", info.c_str());
                    continue;
                }

                _defaultValues[strings[1]] = strings[2];
            }

            for (const auto &name: SHADER_PART_NAMES)
            {
                if (length >= 2 && strings[0] == name)
                    _additionalParameters[name] = strings[1];
            }

            searchStart = match.suffix().first;
        }
    }

    shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords)
    {
        auto   shaderSource = Utils::ReadFileWithIncludes(_path);
        string keywordsDirectives;
        for (const auto &keyword: _keywords)
            keywordsDirectives += "#define " + keyword + "\n";

        unordered_map<string, string> defaultValues {};
        unordered_map<string, string> additionalParameters {};
        ParsePragmas(shaderSource, defaultValues, additionalParameters);

        bool   success = true;
        GLuint shaderParts[SHADER_PART_COUNT];
        size_t shaderPartCount = 0;

        for (int i = 0; i < SHADER_PART_COUNT; ++i)
        {
            auto        part     = SHADER_PARTS[i];
            const auto &partName = SHADER_PART_NAMES[i];
            if (!additionalParameters.contains(partName))
                continue;

            GLuint shaderPart   = 0;
            auto & relativePath = additionalParameters[partName];
            auto   partPath     = _path.parent_path() / relativePath;
            auto   partSource   = Utils::ReadFileWithIncludes(partPath);
            success &= TryCompileShaderPart(part, partPath, partSource, keywordsDirectives, shaderPart);

            shaderParts[shaderPartCount++] = shaderPart;
        }

        GLuint program;
        success &= TryLinkProgram(span<GLuint> {shaderParts, shaderPartCount}, program, _path);

        return success ? shared_ptr<Shader>(new Shader(program, defaultValues)) : nullptr;
    }

#pragma endregion

} // namespace ShaderLoader