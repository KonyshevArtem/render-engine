#ifndef OPENGL_STUDY_SHADER_LOADER_H
#define OPENGL_STUDY_SHADER_LOADER_H

#include <filesystem>
#include <memory>
#include <string>

class Shader;

using namespace std;

namespace ShaderLoader
{
    shared_ptr<Shader> Load(const filesystem::path &_path, const vector<string> &_keywords);
}; // namespace ShaderLoader

#endif //OPENGL_STUDY_SHADER_LOADER_H
