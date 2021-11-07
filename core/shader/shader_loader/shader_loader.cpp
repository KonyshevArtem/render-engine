#include "shader_loader.h"
#include "../../../utils/utils.h"
#include "../../graphics/graphics.h"
#include "../shader.h"
#include <boost/algorithm/string.hpp>
#include <regex>

//region public methods

shared_ptr<Shader> ShaderLoader::Load(const filesystem::path &_path, const vector<string> &_keywords, bool _silent) // NOLINT(misc-no-recursion)
{
    GLuint vertexPart;
    GLuint fragmentPart;
    GLuint program;

    string shaderSource = Utils::ReadFileWithIncludes(_path);
    string keywordsDirectives;
    for (const auto &keyword: _keywords)
        keywordsDirectives += "#define " + keyword + "\n";

    bool success = ShaderLoader::TryCompileShaderPart(GL_VERTEX_SHADER, _path, shaderSource, keywordsDirectives, vertexPart);
    success &= ShaderLoader::TryCompileShaderPart(GL_FRAGMENT_SHADER, _path, shaderSource, keywordsDirectives, fragmentPart);
    success &= ShaderLoader::TryLinkProgram(vertexPart, fragmentPart, program, _path);

    if (!success)
    {
        if (_silent)
            return Shader::GetFallbackShader();
        else
            exit(1);
    }

    unordered_map<string, string> defaultValues {};
    ParseAdditionalInfo(shaderSource, defaultValues);

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
                                        const string &_path,
                                        const string &_source,
                                        const string &_keywordDirectives,
                                        GLuint       &_outShaderPart)
{
    const string &globalDirectives = Graphics::GetGlobalShaderDirectives();
    const string &shaderPartCode   = GetShaderPartCode(_shaderPartType);
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

const string &ShaderLoader::GetShaderPartCode(GLuint _shaderPartType)
{
    if (!m_ShaderPartCode.contains(_shaderPartType))
    {
        switch (_shaderPartType)
        {
            case GL_VERTEX_SHADER:
            {
                m_ShaderPartCode[_shaderPartType] = "\nout Varyings vars;\nvoid main(){vars=VERTEX();gl_Position=vars.PositionCS;}\n";
                break;
            }
            case GL_FRAGMENT_SHADER:
            {
                m_ShaderPartCode[_shaderPartType] = "\nin Varyings vars;\nout vec4 outColor;\nvoid main(){outColor=FRAGMENT(vars);}\n";
                break;
            }
            default:
            {
                m_ShaderPartCode[_shaderPartType] = "";
                break;
            }
        }
    }

    return m_ShaderPartCode.at(_shaderPartType);
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

void ShaderLoader::ParseAdditionalInfo(const string &_shaderSource, unordered_map<string, string> &_defaultValues)
{
    smatch                 match;
    regex                  expression(R"(\s*#pragma\s+(.+)\n)");
    string::const_iterator searchStart(_shaderSource.cbegin());

    while (regex_search(searchStart, _shaderSource.cend(), match, expression))
    {
        string         info = match[1].str();
        vector<string> strings;
        boost::split(strings, info, boost::is_any_of(" "));

        if (strings[0] == "default")
        {
            if (strings.size() < 3)
            {
                fprintf(stderr, "Default value info has incorrect format: %s\n", info.c_str());
                continue;
            }

            _defaultValues[strings[1]] = strings[2];
        }

        searchStart = match.suffix().first;
    }
}

//endregion
