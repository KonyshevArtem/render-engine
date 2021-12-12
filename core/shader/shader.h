#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "uniform_type/uniform_type.h"
#include <GLUT/glut.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Texture;
class BaseUniform;

class Shader
{
#pragma region construction

public:
    static shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords);

    explicit Shader(GLuint _program, unordered_map<string, string> _defaultValues);
    ~Shader();

#pragma endregion

#pragma region fields

private:
    GLuint                                                                               m_Program;
    unordered_map<string, shared_ptr<BaseUniform>>                                       m_Uniforms;
    unordered_map<string, int>                                                           m_TextureUnits;
    unordered_map<string, string>                                                        m_DefaultValues;
    inline static unordered_map<string, shared_ptr<Texture>>                             m_GlobalTextures  = {};
    inline static unordered_map<string, unordered_map<UniformType, shared_ptr<Texture>>> m_DefaultTextures = {};
    inline static const Shader *                                                         m_CurrentShader   = nullptr;

#pragma endregion

#pragma region public methods

public:
    void Use() const;
    void SetUniform(const string &_name, const void *_data) const;
    void SetTextureUniform(const string &_name, const shared_ptr<Texture> &_texture) const;

    static void DetachCurrentShader();
    static void SetGlobalTexture(const string &_name, shared_ptr<Texture> _texture);

#pragma endregion

#pragma region service methods

private:
    static void InitDefaultTextures();
    void        SetDefaultValues() const;

#pragma endregion

    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H