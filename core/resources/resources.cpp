#include "resources.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "editor/profiler/profiler.h"
#include "texture/texture_binary_reader.h"
#include "material/material.h"
#include "material/material_parser.h"
#include "fbx_asset/fbx_asset.h"
#include "file_system/file_system.h"
#include "debug.h"

template<>
std::shared_ptr<Texture2D> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<Texture2D>", path.string());

    TextureBinaryReader reader;
    if (!reader.ReadTexture(path))
        return nullptr;

    const TextureHeader& header = reader.GetHeader();

    std::shared_ptr<Texture2D> texture = std::shared_ptr<Texture2D>(new Texture2D(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear, false, path.string()));
    UploadPixels(*texture, 1, header.MipCount, reader);
    return texture;
}

template<>
std::shared_ptr<Cubemap> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<Cubemap>", path.string());

    TextureBinaryReader reader;
    if (!reader.ReadTexture(path))
        return nullptr;

    const TextureHeader& header = reader.GetHeader();

    constexpr int facesCount = static_cast<int>(CubemapFace::MAX);
    if (header.Depth != facesCount)
    {
        Debug::LogErrorFormat("Number of slices in texture file is {}, expected {}", std::to_string(header.Depth), std::to_string(facesCount));
        return nullptr;
    }

    std::shared_ptr<Cubemap> cubemap = std::shared_ptr<Cubemap>(new Cubemap(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear, path.string()));
    UploadPixels(*cubemap, facesCount, header.MipCount, reader);
    return cubemap;
}

template<>
std::shared_ptr<Material> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<Material>", path.string());
    return MaterialParser::Parse(path);
}

template<>
std::shared_ptr<FBXAsset> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<FBXAsset>", path.string());

    std::vector<uint8_t> content;
    FileSystem::ReadFileBytes(FileSystem::GetResourcesPath() / path, content);

    auto *scene = ofbx::load(&content[0], content.size(), static_cast<ofbx::u64>(ofbx::LoadFlags::TRIANGULATE)); // NOLINT(cppcoreguidelines-narrowing-conversions)
    if (!scene)
        return nullptr;

    auto asset = std::shared_ptr<FBXAsset>(new FBXAsset(scene, path.string()));

    scene->destroy();
    return asset;
}

void Resources::UploadPixels(Texture& texture, int facesCount, int mipCount, TextureBinaryReader& reader)
{
    for (int face = 0; face < facesCount; ++face)
    {
        for (int mip = 0; mip < mipCount; ++mip)
        {
            std::span<uint8_t> pixels = reader.GetPixels(face, mip);
            texture.UploadPixels(pixels.data(), pixels.size(), 0, mip, static_cast<CubemapFace>(face));
        }
    }
}