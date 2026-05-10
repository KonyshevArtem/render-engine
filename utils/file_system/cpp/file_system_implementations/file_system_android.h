#ifndef RENDERENGINE_FILE_SYSTEM_ANDROID_H
#define RENDERENGINE_FILE_SYSTEM_ANDROID_H

#if RENDER_ENGINE_ANDROID

#include "file_system_base.h"

struct AAssetManager;

class FileSystemAndroid : public FileSystemBase
{
public:
    explicit FileSystemAndroid(void* fileSystemData);

    virtual bool FileExists(const std::filesystem::path& path) override;
    virtual std::string ReadFile(const std::filesystem::path& path) override;
    virtual bool ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& bytes) override;
    virtual void WriteFile(const std::filesystem::path& path, const std::string& content) override;

private:
    AAssetManager* m_AssetManager;
};

#endif

#endif //RENDERENGINE_FILE_SYSTEM_ANDROID_H
