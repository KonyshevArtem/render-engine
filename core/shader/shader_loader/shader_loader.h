#ifndef OPENGL_STUDY_SHADER_LOADER_H
#define OPENGL_STUDY_SHADER_LOADER_H

#include <OpenGL/gl3.h>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

class Shader;

using namespace std;

class ShaderLoader
{
    //region constants

    const static string VERTEX_PART_REGEX_FORMAT;
    const static string FRAGMENT_PART_REGEX_FORMAT;

    const static string VERTEX_PART_CODE_FORMAT;
    const static string FRAGMENT_PART_CODE_FORMAT;

    //endregion

    //region fields

private:
    inline static unordered_map<GLuint, string> m_ShaderPartCode = {};
    inline static shared_ptr<Shader>            m_FallbackShader = nullptr;

    //endregion

    //region public methods

public:
    static shared_ptr<Shader>        Load(const filesystem::path &_path, const vector<string> &_keywords, bool _silent = true);
    static const shared_ptr<Shader> &GetFallbackShader();

    //endregion

    // region service methods

private:
    static bool TryCompileShaderPart(GLuint        _shaderPartType,
                                     const string &_shaderPartFunctionName,
                                     const string &_path,
                                     const string &_source,
                                     const string &_keywordDirectives,
                                     GLuint       &_outShaderPart);

    static bool TryLinkProgram(GLuint        _vertexPart,
                               GLuint        _fragmentPart,
                               GLuint       &_program,
                               const string &_path);

    static string GetShaderPartCode(GLuint        _shaderPartType,
                                    const string &_source,
                                    const string &_shaderPartFunctionName);

    static void ParseAdditionalInfo(const string                  &_shaderSource,
                                    unordered_map<string, string> &_defaultValues,
                                    unordered_map<GLuint, string> &_shaderPartsFunctionNames);

    //endregion
};

#endif //OPENGL_STUDY_SHADER_LOADER_H
