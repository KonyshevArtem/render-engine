#ifndef OPENGL_STUDY_UTILS_H
#define OPENGL_STUDY_UTILS_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define CHECK_GL_ERROR printf("%s\n", gluErrorString(glGetError()));
#pragma clang diagnostic pop

#include "filesystem"
#include "string"

using namespace std;

namespace Utils
{
    string                  ReadFile(const filesystem::path &_relativePath);
    string                  ReadFileWithIncludes(const filesystem::path &_relativePath);
    void                    WriteFile(const filesystem::path &_relativePath, const string &_content);
    const filesystem::path &GetExecutableDirectory();
}; // namespace Utils

#endif //OPENGL_STUDY_UTILS_H