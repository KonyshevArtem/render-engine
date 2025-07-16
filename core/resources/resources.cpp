#include "resources.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "editor/profiler/profiler.h"
#include "texture/texture_binary_reader.h"
#include "material/material.h"
#include "material/material_parser.h"
#include "mesh/mesh.h"
#include "mesh/mesh_binary_reader.h"
#include "file_system/file_system.h"
#include "resource.h"
#include "debug.h"

std::unordered_map<std::filesystem::path, std::shared_ptr<Resource>> Resources::s_LoadedResources;

template<>
std::shared_ptr<Texture2D> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<Texture2D>", path.string());

    if (s_LoadedResources.contains(path))
        return std::dynamic_pointer_cast<Texture2D>(s_LoadedResources.at(path));

    TextureBinaryReader reader;
    if (!reader.ReadTexture(path))
        return nullptr;

    const TextureHeader& header = reader.GetHeader();

    std::shared_ptr<Texture2D> texture = std::shared_ptr<Texture2D>(new Texture2D(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear, false, path.string()));
    UploadPixels(*texture, 1, header.MipCount, reader);

    s_LoadedResources[path] = texture;

    return texture;
}

template<>
std::shared_ptr<Cubemap> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<Cubemap>", path.string());

    if (s_LoadedResources.contains(path))
        return std::dynamic_pointer_cast<Cubemap>(s_LoadedResources.at(path));

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

    s_LoadedResources[path] = cubemap;

    return cubemap;
}

template<>
std::shared_ptr<Material> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<Material>", path.string());

    if (s_LoadedResources.contains(path))
        return std::dynamic_pointer_cast<Material>(s_LoadedResources.at(path));

    std::shared_ptr<Material> material = MaterialParser::Parse(path);
    s_LoadedResources[path] = material;
    return material;
}

void Resources::UnloadAllResources()
{
    s_LoadedResources.clear();
}

template<>
std::shared_ptr<Mesh> Resources::Load(const std::filesystem::path& path)
{
    Profiler::Marker _("Resources::Load<FBXAsset>", path.string());

    if (s_LoadedResources.contains(path))
        return std::dynamic_pointer_cast<Mesh>(s_LoadedResources.at(path));

    MeshBinaryReader reader;
    if (!reader.ReadMesh(path))
        return nullptr;

    std::vector<uint8_t> content;
    FileSystem::ReadFileBytes(FileSystem::GetResourcesPath() / path, content);

    const MeshHeader& header = reader.GetHeader();
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(reader.GetVertexData(), reader.GetIndices(), header.HasUV, header.HasNormals, header.HasTangents,
                                                        header.MinPoint, header.MaxPoint, header.Name);

    s_LoadedResources[path] = mesh;

    return mesh;
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