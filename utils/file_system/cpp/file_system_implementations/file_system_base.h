#ifndef RENDER_ENGINE_FILE_SYSTEM_BASE_H
#define RENDER_ENGINE_FILE_SYSTEM_BASE_H

#include "special_folder.h"

#include <string>
#include <filesystem>
#include <span>
#include <unordered_map>

class FileSystemBase
{
public:
    FileSystemBase() = default;

    virtual bool FileExists(const std::filesystem::path& path);
    virtual std::string ReadFile(const std::filesystem::path& path);
    virtual bool ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& bytes);
    virtual void WriteFile(const std::filesystem::path& path, const std::string& content);
    virtual void WriteFileBytes(const std::filesystem::path& path, const std::span<const uint8_t>& bytes);

    const std::filesystem::path& GetBuildResourcesPath();
	const std::filesystem::path& GetEditorResourcesPath();
    const std::filesystem::path& GetSpecialFolderPath(SpecialFolder folder);

protected:
    std::filesystem::path m_BuildResourcesPath;
	std::filesystem::path m_EditorResourcesPath;
    std::unordered_map<SpecialFolder, std::filesystem::path> m_SpecialFolderPaths;

    virtual std::filesystem::path GetSpecialFolderPath_Internal(SpecialFolder folder);
};

#endif //RENDER_ENGINE_FILE_SYSTEM_BASE_H
