#include "resources.h"
#include "texture/texture.h"
#include "editor/profiler/profiler.h"
#include "texture/texture_binary_reader.h"
#include "material/material.h"
#include "material/material_parser.h"
#include "mesh/mesh.h"
#include "mesh/mesh_binary_reader.h"
#include "font/font.h"
#include "file_system.h"
#include "resource.h"
#include "shader/shader_loader/shader_loader.h"
#include "debug.h"

std::unordered_map<std::string, std::shared_ptr<Resource>> Resources::s_LoadedResources;
std::unordered_map<std::string, Resources::AsyncLoadRequest> Resources::s_AsyncLoadRequests;

std::shared_mutex Resources::s_LoadedResourcesMutex;
std::shared_mutex Resources::s_AsyncLoadRequestsMutex;

template<>
std::shared_ptr<Texture> Resources::Load(const std::filesystem::path& path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Texture>", path.string());

    const std::string cacheKey = path.string();

    std::shared_ptr<Texture> texture;
    if (TryGetFromCache(cacheKey, texture))
        return texture;

    TextureBinaryReader reader;
    if (!reader.ReadTexture(path))
    {
        Debug::LogErrorFormat("[Resources] Cannot load texture: {}", path.string());
        return nullptr;
    }

    const TextureHeader& header = reader.GetHeader();

	int facesCount = 1;
    if (header.Type == TextureType::TEXTURE_CUBEMAP)
    {
        facesCount = static_cast<int>(CubemapFace::MAX);
        if (header.Depth != facesCount)
        {
            Debug::LogErrorFormat("[Resources] Number of slices in cubemap {} is {}, expected {}", path.string(), std::to_string(header.Depth), std::to_string(facesCount));
            return nullptr;
        }
    }

    GraphicsBackendTextureDescriptor descriptor;
	descriptor.Type = header.Type;
    descriptor.Width = header.Width;
    descriptor.Height = header.Height;
    descriptor.MipLevels = header.MipCount;
    descriptor.Linear = header.IsLinear;
    descriptor.Format = header.TextureFormat;

    texture = std::make_shared<Texture>(descriptor, path.string());
    UploadPixels(*texture, facesCount, header.MipCount, reader);
    AddToCache(cacheKey, texture);

    return texture;
}

template<>
std::shared_ptr<Material> Resources::Load(const std::filesystem::path& path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Material>", path.string());

    const std::string cacheKey = path.string();

    std::shared_ptr<Material> material;
    if (TryGetFromCache(cacheKey, material))
        return material;

    material = MaterialParser::Parse(path, asyncSubresourceLoads);
    AddToCache(cacheKey, material);
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

    const std::string cacheKey = path.string();

    std::shared_ptr<Mesh> mesh;
    if (TryGetFromCache(cacheKey, mesh))
        return mesh;

    MeshBinaryReader reader;
    if (!reader.ReadMesh(path))
    {
        Debug::LogErrorFormat("[Resources] Cannot load mesh: {}", path.string());
        return nullptr;
    }

    const MeshHeader& header = reader.GetHeader();
    mesh = std::make_shared<Mesh>(reader.GetVertexData(), reader.GetIndices(), header.HasUV, header.HasNormals, header.HasTangents,
                                                        header.MinPoint, header.MaxPoint, header.Name);

    AddToCache(cacheKey, mesh);

    return mesh;
}

template<>
std::shared_ptr<Font> Resources::Load(const std::filesystem::path &path, bool asyncSubresourceLoads)
{
    Profiler::Marker _("Resources::Load<Font>", path.string());

    const std::string cacheKey = path.string();

    std::shared_ptr<Font> font;
    if (TryGetFromCache(cacheKey, font))
        return font;

    std::vector<uint8_t> bytes;
    if (!FileSystem::ReadFileBytes(FileSystem::GetBuildResourcesPath() / path, bytes))
    {
        Debug::LogErrorFormat("[Resources] Cannot load font: {}", path.string());
        return nullptr;
    }

    font = std::make_shared<Font>(bytes, path.string());
    AddToCache(cacheKey, font);

    return font;
}

std::shared_ptr<Shader> Resources::LoadShader(const std::filesystem::path& path, const std::vector<std::string>& defines, bool reload)
{
    Profiler::Marker _("Resources::Load<Shader>", path.string());

    const bool shaderDebug = Arguments::Contains("-shaderdebug");

    const std::string cacheKey = path.string() + ShaderLoader::GetShaderHash(ShaderLoader::GetDefinesHash(defines), shaderDebug);

    std::shared_ptr<Shader> shader;
    if (!reload && TryGetFromCache(cacheKey, shader))
        return shader;

    shader = std::make_shared<Shader>(path, defines);
    AddToCache(cacheKey, shader);

    return shader;
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

void Resources::AddToCache(const std::string& cacheKey, std::shared_ptr<Resource> resource)
{
    std::unique_lock lock(s_LoadedResourcesMutex);
    s_LoadedResources[cacheKey] = resource;
}