#ifndef RENDER_ENGINE_RESOURCES_H
#define RENDER_ENGINE_RESOURCES_H

#include <memory>
#include <filesystem>
#include <unordered_map>

class Resource;
class Texture;
class TextureBinaryReader;

class Resources
{
public:
    template<typename T>
    static std::shared_ptr<T> Load(const std::filesystem::path& path);

    static void UnloadAllResources();

private:
    static void UploadPixels(Texture& texture, int facesCount, int mipCount, TextureBinaryReader& reader);

    static std::unordered_map<std::filesystem::path, std::shared_ptr<Resource>> s_LoadedResources;
};

#endif //RENDER_ENGINE_RESOURCES_H
