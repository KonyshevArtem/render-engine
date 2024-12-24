#ifndef RENDER_ENGINE_FILE_SYSTEM_H
#define RENDER_ENGINE_FILE_SYSTEM_H

#include <string>
#include <filesystem>
#include <vector>

namespace FileSystem
{
    std::string ReadFile(const std::filesystem::path& path);
    std::string ReadFileWithIncludes(const std::filesystem::path& path);
    bool        ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& bytes);
    void        WriteFile(const std::filesystem::path& path, const std::string& content);

    const std::filesystem::path& GetExecutablePath();
    const std::filesystem::path& GetResourcesPath();
}

#endif //RENDER_ENGINE_FILE_SYSTEM_H
