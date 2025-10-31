#include "resources.h"
#include "texture_2d/texture_2d.h"
#include "texture_2d_array/texture_2d_array.h"
#include "cubemap/cubemap.h"
#include "editor/profiler/profiler.h"
#include "texture/texture_binary_reader.h"
#include "material/material.h"
#include "material/material_parser.h"
#include "mesh/mesh.h"
#include "mesh/mesh_binary_reader.h"
#include "font/font.h"
#include "font/font_binary_reader.h"
#include "file_system/file_system.h"
#include "resource.h"
#include "debug.h"

std::unordered_map<std::filesystem::path, std::shared_ptr<Resource>> Resources::s_LoadedResources;
std::unordered_map<std::filesystem::path, Resources::AsyncLoadRequest> Resources::s_AsyncLoadRequests;

std::shared_mutex Resources::s_LoadedResourcesMutex;
std::shared_mutex Resources::s_AsyncLoadRequestsMutex;

template<>
std::shared_ptr<Texture2D> Resources::Load(const std::filesystem::path& path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Texture2D>", path.string());

    std::shared_ptr<Texture2D> texture;
    if (TryGetFromCache(path, texture))
        return texture;

    TextureBinaryReader reader;
    if (!reader.ReadTexture(path))
        return nullptr;

    const TextureHeader& header = reader.GetHeader();

    texture = std::shared_ptr<Texture2D>(new Texture2D(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear, false, path.string()));
    UploadPixels(*texture, 1, header.MipCount, reader);
    AddToCache(path, texture);

    return texture;
}

template<>
std::shared_ptr<Cubemap> Resources::Load(const std::filesystem::path& path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Cubemap>", path.string());

    std::shared_ptr<Cubemap> cubemap;
    if (TryGetFromCache(path, cubemap))
        return cubemap;

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

    cubemap = std::shared_ptr<Cubemap>(new Cubemap(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear, path.string()));
    UploadPixels(*cubemap, facesCount, header.MipCount, reader);
    AddToCache(path, cubemap);

    return cubemap;
}

template<>
std::shared_ptr<Material> Resources::Load(const std::filesystem::path& path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Material>", path.string());

    std::shared_ptr<Material> material;
    if (TryGetFromCache(path, material))
        return material;

    material = MaterialParser::Parse(path, asyncSubresourceLoads);
    AddToCache(path, material);
    return material;
}

void Resources::UnloadAllResources()
{
    s_LoadedResources.clear();
}

template<>
std::shared_ptr<Mesh> Resources::Load(const std::filesystem::path& path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Mesh>", path.string());

    std::shared_ptr<Mesh> mesh;
    if (TryGetFromCache(path, mesh))
        return mesh;

    MeshBinaryReader reader;
    if (!reader.ReadMesh(path))
        return nullptr;

    const MeshHeader& header = reader.GetHeader();
    mesh = std::make_shared<Mesh>(reader.GetVertexData(), reader.GetIndices(), header.HasUV, header.HasNormals, header.HasTangents,
                                                        header.MinPoint, header.MaxPoint, header.Name);

    AddToCache(path, mesh);

    return mesh;
}

template<>
std::shared_ptr<Font> Resources::Load(const std::filesystem::path &path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Font>", path.string());

    std::shared_ptr<Font> font;
    if (TryGetFromCache(path, font))
        return font;

    FontBinaryReader reader;
    if (!reader.ReadFont(path))
        return nullptr;

    const TextureHeader& header = reader.PagesHeader;
    std::shared_ptr<Texture2DArray> pagesTexture = Texture2DArray::Create(header.Width, header.Height, header.Depth, header.TextureFormat, header.IsLinear, false, path.string());
    UploadPixels(*pagesTexture, reader);

    font = std::make_shared<Font>(reader.Common, reader.Chars, reader.KerningPairs, pagesTexture);
    AddToCache(path, font);

    return font;
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

void Resources::UploadPixels(Texture& texture, FontBinaryReader& reader)
{
    for (int i = 0; i < reader.Common.Pages; ++i)
        texture.UploadPixels(reader.PageBytes[i].data(), reader.PageBytes[i].size_bytes(), i, 0);
}

void Resources::AddToCache(const std::filesystem::path& path, std::shared_ptr<Resource> resource)
{
    std::unique_lock lock(s_LoadedResourcesMutex);
    s_LoadedResources[path] = resource;
}