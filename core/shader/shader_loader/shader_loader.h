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
#pragma region constants

    const static GLuint                        SUPPORTED_SHADER_PARTS[];
    const static unordered_map<GLuint, string> SHADER_PART_NAMES;

#pragma endregion

#pragma region public methods

public:
    static shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords);

#pragma endregion

#pragma region service methods

private:
    static bool TryCompileShaderPart(GLuint        _shaderPartType,
                                     const string &_path,
                                     const string &_source,
                                     const string &_keywordDirectives,
                                     GLuint &      _outShaderPart);

    static bool TryLinkProgram(const vector<GLuint> &_shaderParts,
                               GLuint &              _program,
                               const string &        _path);

    static void ParsePragmas(const string &                 _shaderSource,
                             unordered_map<string, string> &_defaultValues,
                             unordered_map<string, string> &_additionalParameters);

#pragma endregion
};

#endif //OPENGL_STUDY_SHADER_LOADER_H
