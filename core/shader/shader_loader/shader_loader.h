#ifndef OPENGL_STUDY_SHADER_LOADER_H
#define OPENGL_STUDY_SHADER_LOADER_H

#include <OpenGL/gl3.h>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>

class Shader;

using namespace std;

namespace ShaderLoader
{
    shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords);
}; // namespace ShaderLoader

#endif //OPENGL_STUDY_SHADER_LOADER_H
