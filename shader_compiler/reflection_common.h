#ifndef RENDER_ENGINE_SHADER_COMPILER_REFLECTION_COMMON_H
#define RENDER_ENGINE_SHADER_COMPILER_REFLECTION_COMMON_H

#include <string>
#include <unordered_map>

enum BufferType
{
    BYTE_ADDRESS_BUFFER,
    STRUCTURED_BUFFER,
    CONSTANT_BUFFER,
    TYPED_BUFFER
};

struct ResourceDescriptor
{
    uint32_t Binding;
};

struct TextureDescriptor : ResourceDescriptor
{
    bool ReadWrite;
};

struct BufferDescriptor : ResourceDescriptor
{
    uint32_t Size;
    BufferType BufferType;
    bool ReadWrite;
    std::unordered_map<std::string, uint32_t> Variables;
};

struct ThreadGroupSize
{
    uint32_t X = 1;
    uint32_t Y = 1;
    uint32_t Z = 1;
};

struct Reflection
{
    std::unordered_map<std::string, BufferDescriptor> Buffers;
    std::unordered_map<std::string, TextureDescriptor> Textures;
    std::unordered_map<std::string, ResourceDescriptor> Samplers;
    ThreadGroupSize ThreadGroupSize;
};

inline void WriteResourceDescriptor(const std::string& resourceName, uint32_t bindPoint, std::unordered_map<std::string, ResourceDescriptor>& resources)
{
    if (resources.contains(resourceName))
        return;

    const ResourceDescriptor desc{ bindPoint };
    resources[resourceName] = desc;
}

inline void WriteTextureDescriptor(const std::string& resourceName, uint32_t bindPoint, bool readWrite, std::unordered_map<std::string, TextureDescriptor>& textures)
{
    if (textures.contains(resourceName))
        return;

    const TextureDescriptor desc{ bindPoint, readWrite };
    textures[resourceName] = desc;
}

inline void WriteBufferDescriptor(const std::string& resourceName, uint32_t bindPoint, uint32_t size, BufferType bufferType, bool readWrite, std::unordered_map<std::string, BufferDescriptor>& buffers)
{
    if (buffers.contains(resourceName))
        return;

    const BufferDescriptor desc{ bindPoint, size, bufferType, readWrite };
    buffers[resourceName] = desc;
}

#endif //RENDER_ENGINE_SHADER_COMPILER_REFLECTION_COMMON_H
