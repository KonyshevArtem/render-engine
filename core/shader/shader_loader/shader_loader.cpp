#include "shader_loader.h"
#include "../../../utils/utils.h"
#include "../../graphics/graphics.h"
#include "../shader.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <regex>

// TODO use syntax analysis instead of regex

#pragma region constants

const GLuint                        ShaderLoader::SUPPORTED_SHADER_PARTS[3] {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER};
const unordered_map<GLuint, string> ShaderLoader::SHADER_PART_NAMES {
        {GL_VERTEX_SHADER, "vertex"},
        {GL_GEOMETRY_SHADER, "geometry"},
        {GL_FRAGMENT_SHADER, "fragment"}};

#pragma endregion

#pragma region public methods

shared_ptr<Shader> ShaderLoader::Load(const filesystem::path &_path, const vector<string> &_keywords)
{
    auto shaderSource = Utils::ReadFileWithIncludes(_path);
    string keywordsDirectives;
    for (const auto &keyword: _keywords)
        keywordsDirectives += "#define " + keyword + "\n";

    unordered_map<string, string> defaultValues {};
    unordered_map<string, string> additionalParameters {};
    ParsePragmas(shaderSource, defaultValues, additionalParameters);

    bool           success = true;
    vector<GLuint> shaderParts;
    for (const auto &part: SUPPORTED_SHADER_PARTS)
    {
        const string &partName = SHADER_PART_NAMES.at(part);
        if (!additionalParameters.contains(partName))
            continue;

        GLuint shaderPart   = 0;
        auto & relativePath = additionalParameters[partName];
        auto   partPath     = _path.parent_path() / relativePath;
        string partSource   = Utils::ReadFileWithIncludes(partPath);
        success &= ShaderLoader::TryCompileShaderPart(part, partPath, partSource, keywordsDirectives, shaderPart);

        shaderParts.push_back(shaderPart);
    }

    GLuint program;
    success &= TryLinkProgram(shaderParts, program, _path);

    return success ? make_shared<Shader>(program, defaultValues) : nullptr;
}

#pragma endregion

#pragma region service methods

bool ShaderLoader::TryCompileShaderPart(GLuint        _shaderPartType,
                                        const string &_path,
                                        const string &_source,
                                        const string &_keywordDirectives,
                                        GLuint &      _outShaderPart)
{
    const string &globalDirectives = Graphics::GetGlobalShaderDirectives();
    int           sourcesCount     = 3;
    const char *  sources[sourcesCount];

    sources[0] = globalDirectives.c_str();
    sources[1] = _keywordDirectives.c_str();
    sources[2] = _source.c_str();

    GLuint shader = glCreateShader(_shaderPartType);
    glShaderSource(shader, sourcesCount, sources, nullptr);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto *logMsg = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, nullptr, logMsg);

        fprintf(stderr, "Shader compilation failed: %s\n%s\n", _path.c_str(), logMsg);

        delete[] logMsg;
        return false;
    }

    _outShaderPart = shader;
    return true;
}

bool ShaderLoader::TryLinkProgram(const vector<GLuint> &_shaderParts, GLuint &_program, const string &_path)
{
    GLuint program = glCreateProgram();

    for (const auto &part: _shaderParts)
    {
        if (glIsShader(part))
            glAttachShader(program, part);
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto *logMsg = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, nullptr, logMsg);
        fprintf(stderr, "Program link failed: %s\n%s", _path.c_str(), logMsg);

        delete[] logMsg;
        return false;
    }

    for (const auto &part: _shaderParts)
    {
        if (glIsShader(part))
        {
            glDetachShader(program, part);
            glDeleteShader(part);
        }
    }

    _program = program;
    return true;
}

void ShaderLoader::ParsePragmas(const string &                 _shaderSource,
                                unordered_map<string, string> &_defaultValues,
                                unordered_map<string, string> &_additionalParameters)
{
    smatch                 match;
    regex                  expression(R"(\s*#pragma\s+(.+)\n)");
    string::const_iterator searchStart(_shaderSource.cbegin());

    while (regex_search(searchStart, _shaderSource.cend(), match, expression))
    {
        string         info = match[1].str();
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

        for (const auto &pair: SHADER_PART_NAMES)
        {
            if (strings[0] == pair.second && length >= 2)
                _additionalParameters[pair.second] = strings[1];
        }

        searchStart = match.suffix().first;
    }
}

#pragma endregion
