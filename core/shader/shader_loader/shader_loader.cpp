#include "shader_loader.h"
#include "../../../utils/utils.h"
#include "../../graphics/graphics.h"
#include "../shader.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <regex>

// TODO use syntax analysis instead of regex

//region constants

const string ShaderLoader::VERTEX_PART_REGEX_FORMAT(R"(\s*(\w+)\s+%1%\(\s*\))");          // NOLINT(cert-err58-cpp)
const string ShaderLoader::FRAGMENT_PART_REGEX_FORMAT(R"(\s*vec4\s+%1%\((\w+)\s+\w+\))"); // NOLINT(cert-err58-cpp)

const string ShaderLoader::VERTEX_PART_CODE_FORMAT("\nout %1% vars;\nvoid main(){vars=%2%();gl_Position=vars.PositionCS;}\n");  // NOLINT(cert-err58-cpp)
const string ShaderLoader::FRAGMENT_PART_CODE_FORMAT("\nin %1% vars;\nout vec4 outColor;\nvoid main(){outColor=%2%(vars);}\n"); // NOLINT(cert-err58-cpp)

//endregion

//region public methods

shared_ptr<Shader> ShaderLoader::Load(const filesystem::path &_path, const vector<string> &_keywords, bool _silent) // NOLINT(misc-no-recursion)
{
    GLuint vertexPart   = 0;
    GLuint fragmentPart = 0;
    GLuint program;

    string shaderSource = Utils::ReadFileWithIncludes(_path);
    string keywordsDirectives;
    for (const auto &keyword: _keywords)
        keywordsDirectives += "#define " + keyword + "\n";

    unordered_map<string, string> defaultValues {};
    unordered_map<GLuint, string> shaderPartFunctionNames {};
    ParseAdditionalInfo(shaderSource, defaultValues, shaderPartFunctionNames);

    bool success = true;
    if (shaderPartFunctionNames.contains(GL_VERTEX_SHADER))
    {
        success &= ShaderLoader::TryCompileShaderPart(GL_VERTEX_SHADER,
                                                      shaderPartFunctionNames[GL_VERTEX_SHADER],
                                                      _path,
                                                      shaderSource,
                                                      keywordsDirectives,
                                                      vertexPart);
    }

    if (shaderPartFunctionNames.contains(GL_FRAGMENT_SHADER))
    {
        success &= ShaderLoader::TryCompileShaderPart(GL_FRAGMENT_SHADER,
                                                      shaderPartFunctionNames[GL_FRAGMENT_SHADER],
                                                      _path,
                                                      shaderSource,
                                                      keywordsDirectives,
                                                      fragmentPart);
    }

    success &= ShaderLoader::TryLinkProgram(vertexPart, fragmentPart, program, _path);

    if (!success)
    {
        if (_silent)
            return Shader::GetFallbackShader();
        else
            exit(1);
    }

    return shared_ptr<Shader>(new Shader(program, defaultValues));
}

const shared_ptr<Shader> &ShaderLoader::GetFallbackShader() // NOLINT(misc-no-recursion)
{
    if (m_FallbackShader == nullptr)
        m_FallbackShader = Load("resources/shaders/fallback.glsl", vector<string>(), false);
    return m_FallbackShader;
}

//endregion

//region service methods

bool ShaderLoader::TryCompileShaderPart(GLuint        _shaderPartType,
                                        const string &_shaderPartFunctionName,
                                        const string &_path,
                                        const string &_source,
                                        const string &_keywordDirectives,
                                        GLuint       &_outShaderPart)
{
    const string &globalDirectives = Graphics::GetGlobalShaderDirectives();
    string        shaderPartCode   = GetShaderPartCode(_shaderPartType, _source, _shaderPartFunctionName);
    int           sourcesCount     = 4;
    const char   *sources[sourcesCount];

    sources[0] = globalDirectives.c_str();
    sources[1] = _keywordDirectives.c_str();
    sources[2] = _source.c_str();
    sources[3] = shaderPartCode.c_str();

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

string ShaderLoader::GetShaderPartCode(GLuint        _shaderPartType,
                                       const string &_source,
                                       const string &_shaderPartFunctionName)
{
    switch (_shaderPartType)
    {
        case GL_VERTEX_SHADER:
        {
            string r = (boost::format(VERTEX_PART_REGEX_FORMAT) % _shaderPartFunctionName).str();
            smatch match;
            if (regex_search(_source.cbegin(), _source.cend(), match, regex(r)))
                return (boost::format(VERTEX_PART_CODE_FORMAT) % match[1] % _shaderPartFunctionName).str();
            break;
        }
        case GL_FRAGMENT_SHADER:
        {
            string r = (boost::format(FRAGMENT_PART_REGEX_FORMAT) % _shaderPartFunctionName).str();
            smatch match;
            if (regex_search(_source.cbegin(), _source.cend(), match, regex(r)))
                return (boost::format(FRAGMENT_PART_CODE_FORMAT) % match[1] % _shaderPartFunctionName).str();
            break;
        }
        default:
            return "";
    }

    return "";
}

bool ShaderLoader::TryLinkProgram(GLuint _vertexPart, GLuint _fragmentPart, GLuint &_program, const string &_path)
{
    GLuint program = glCreateProgram();

    bool hasVertex   = glIsShader(_vertexPart);
    bool hasFragment = glIsShader(_fragmentPart);

    if (hasVertex)
        glAttachShader(program, _vertexPart);
    if (hasFragment)
        glAttachShader(program, _fragmentPart);

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

    if (hasVertex)
    {
        glDetachShader(program, _vertexPart);
        glDeleteShader(_vertexPart);
    }

    if (hasFragment)
    {
        glDetachShader(program, _fragmentPart);
        glDeleteShader(_fragmentPart);
    }

    _program = program;
    return true;
}

void ShaderLoader::ParseAdditionalInfo(const string                  &_shaderSource,
                                       unordered_map<string, string> &_defaultValues,
                                       unordered_map<GLuint, string> &_shaderPartFunctionNames)
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
        else if (strings[0] == "vertex")
        {
            if (length >= 2)
                _shaderPartFunctionNames[GL_VERTEX_SHADER] = strings[1];
        }
        else if (strings[0] == "geometry")
        {
            if (length >= 2)
                _shaderPartFunctionNames[GL_GEOMETRY_SHADER] = strings[1];
        }
        else if (strings[0] == "fragment")
        {
            if (length >= 2)
                _shaderPartFunctionNames[GL_FRAGMENT_SHADER] = strings[1];
        }

        searchStart = match.suffix().first;
    }
}

//endregion
