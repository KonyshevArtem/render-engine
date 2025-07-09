#ifndef RENDER_ENGINE_RESOURCES_H
#define RENDER_ENGINE_RESOURCES_H

#include <memory>
#include <filesystem>

class Texture;
class TextureBinaryReader;

class Resources
{
public:
    template<typename T>
    static std::shared_ptr<T> Load(const std::filesystem::path& path);

private:
    static void UploadPixels(Texture& texture, int facesCount, int mipCount, TextureBinaryReader& reader);
};

#endif //RENDER_ENGINE_RESOURCES_H
