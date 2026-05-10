#include "file_system.h"
#include "file_system_implementations/file_system_base.h"
#include "file_system_implementations/file_system_windows.h"
#include "file_system_implementations/file_system_apple.h"
#include "file_system_implementations/file_system_android.h"

namespace FileSystem
{
    FileSystemBase* s_FileSystem;

    void Init(void* fileSystemData)
    {
#if RENDER_ENGINE_WINDOWS
        s_FileSystem = new FileSystemWindows();
#elif RENDER_ENGINE_APPLE
        s_FileSystem = new FileSystemApple();
#elif RENDER_ENGINE_ANDROID
        s_FileSystem = new FileSystemAndroid(fileSystemData);
#endif
    }

    bool FileExists(const std::filesystem::path& path)
    {
        return s_FileSystem->FileExists(path);
    }

    std::string ReadFile(const std::filesystem::path& path)
    {
        return s_FileSystem->ReadFile(path);
    }

    bool ReadFileBytes(const std::filesystem::path& relativePath, std::vector<uint8_t>& bytes)
    {
        return s_FileSystem->ReadFileBytes(relativePath, bytes);
    }

    void WriteFile(const std::filesystem::path& path, const std::string& content)
    {
        s_FileSystem->WriteFile(path, content);
    }

    void WriteFileBytes(const std::filesystem::path& path, const std::span<const uint8_t>& bytes)
    {
        s_FileSystem->WriteFileBytes(path, bytes);
    }

    const std::filesystem::path& GetBuildResourcesPath()
    {
        return s_FileSystem->GetBuildResourcesPath();
    }

    const std::filesystem::path& GetEditorResourcesPath()
    {
        return s_FileSystem->GetEditorResourcesPath();
	}

    const std::filesystem::path& GetSpecialFolderPath(SpecialFolder folder)
    {
        return s_FileSystem->GetSpecialFolderPath(folder);
    }
}
