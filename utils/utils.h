#ifndef OPENGL_STUDY_UTILS_H
#define OPENGL_STUDY_UTILS_H

#include <filesystem>
#include <string>

using namespace std;

namespace Utils
{
    string                  ReadFile(const filesystem::path &_relativePath);
    string                  ReadFileWithIncludes(const filesystem::path &_relativePath);
    void                    WriteFile(const filesystem::path &_relativePath, const string &_content);
    const filesystem::path &GetExecutableDirectory();
}; // namespace Utils

#endif //OPENGL_STUDY_UTILS_H