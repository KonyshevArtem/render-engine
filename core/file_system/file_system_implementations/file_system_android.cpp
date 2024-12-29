#if RENDER_ENGINE_ANDROID

#include "file_system_android.h"

#include <android/asset_manager.h>
#include <exception>

FileSystemAndroid::FileSystemAndroid(void *fileSystemData) : FileSystemBase(),
    m_AssetManager(static_cast<AAssetManager*>(fileSystemData))
{
}

bool FileSystemAndroid::FileExists(const std::filesystem::path& path)
{
    try
    {
        AAsset* asset = AAssetManager_open(m_AssetManager, path.c_str(), AASSET_MODE_UNKNOWN);
        if (asset)
            AAsset_close(asset);
        return asset != nullptr;
    }
    catch(std::exception&)
    {
        return false;
    }
}

std::string FileSystemAndroid::ReadFile(const std::filesystem::path& path)
{
    std::string result;
    AAsset* asset = AAssetManager_open(m_AssetManager, path.c_str(), AASSET_MODE_BUFFER);

    off_t size =  AAsset_getLength(asset);
    result.resize(size);

    const void* data = AAsset_getBuffer(asset);
    memcpy(result.data(), data, size);
    AAsset_close(asset);

    return result;
}

bool FileSystemAndroid::ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& bytes)
{
    AAsset* asset = AAssetManager_open(m_AssetManager, path.c_str(), AASSET_MODE_BUFFER);

    off_t size =  AAsset_getLength(asset);
    bytes.resize(size);

    const void* data = AAsset_getBuffer(asset);
    memcpy(bytes.data(), data, size);
    AAsset_close(asset);

    return true;
}

void FileSystemAndroid::WriteFile(const std::filesystem::path& path, const std::string& content)
{
    throw std::runtime_error("Not implemented");
}

#endif