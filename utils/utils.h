#ifndef OPENGL_STUDY_UTILS_H
#define OPENGL_STUDY_UTILS_H

#include <filesystem>
#include <string>

namespace Utils
{
    std::string                  ReadFile(const std::filesystem::path &_relativePath);
    std::string                  ReadFileWithIncludes(const std::filesystem::path &_relativePath);
    void                         WriteFile(const std::filesystem::path &_relativePath, const std::string &_content);
    const std::filesystem::path &GetExecutableDirectory();
}; // namespace Utils

#endif //OPENGL_STUDY_UTILS_H