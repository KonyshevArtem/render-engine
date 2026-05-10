#ifndef RENDER_ENGINE_FILE_SYSTEM_H
#define RENDER_ENGINE_FILE_SYSTEM_H

#include "special_folder.h"

#include <string>
#include <filesystem>
#include <vector>
#include <span>

namespace FileSystem
{
    void Init(void* fileSystemData);

    bool FileExists(const std::filesystem::path& path);
    std::string ReadFile(const std::filesystem::path& path);
    bool ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& bytes);
    void WriteFile(const std::filesystem::path& path, const std::string& content);
    void WriteFileBytes(const std::filesystem::path& path, const std::span<const uint8_t>& bytes);

    const std::filesystem::path& GetBuildResourcesPath();
    const std::filesystem::path& GetEditorResourcesPath();
    const std::filesystem::path& GetSpecialFolderPath(SpecialFolder folder);
}

#endif //RENDER_ENGINE_FILE_SYSTEM_H
