#ifndef RENDER_ENGINE_FILE_SYSTEM_BASE_H
#define RENDER_ENGINE_FILE_SYSTEM_BASE_H

#include <string>
#include <filesystem>

class FileSystemBase
{
public:
    FileSystemBase() = default;

    virtual bool FileExists(const std::filesystem::path& path);
    virtual std::string ReadFile(const std::filesystem::path& path);
    virtual bool ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& bytes);
    virtual void WriteFile(const std::filesystem::path& path, const std::string& content);

    const std::filesystem::path& GetBuildResourcesPath();
	const std::filesystem::path& GetEditorResourcesPath();

protected:
    std::filesystem::path m_BuildResourcesPath;
	std::filesystem::path m_EditorResourcesPath;
};

#endif //RENDER_ENGINE_FILE_SYSTEM_BASE_H
