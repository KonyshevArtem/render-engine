#ifndef RENDER_ENGINE_UTILS_H
#define RENDER_ENGINE_UTILS_H

#include <vector>
#include <filesystem>
#include <string>

namespace Utils
{
    std::string                  ReadFile(const std::filesystem::path &_relativePath);
    std::string                  ReadFileWithIncludes(const std::filesystem::path &_relativePath);
    bool                         ReadFileBytes(const std::filesystem::path &_relativePath, std::vector<uint8_t> &bytes);
    void                         WriteFile(const std::filesystem::path &_relativePath, const std::string &_content);
    const std::filesystem::path &GetExecutableDirectory();
}; // namespace Utils

#endif //RENDER_ENGINE_UTILS_H