#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_VIEW_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_VIEW_DESCRIPTOR_H

#include "enums/texture_internal_format.h"

struct GraphicsBackendBufferViewDescriptor
{
    uint32_t Size;
    uint32_t Offset;
    uint32_t ElementsCount;
    BufferType Type = BufferType::TYPED_BUFFER;
    TextureInternalFormat Format = TextureInternalFormat::R32F;
    bool ReadWrite = false;

    static GraphicsBackendBufferViewDescriptor Typed(TextureInternalFormat format, uint32_t elementsCount, uint32_t offset, bool readWrite)
    {
        GraphicsBackendBufferViewDescriptor descriptor{};
        descriptor.Type = BufferType::TYPED_BUFFER;
        descriptor.Format = format;
        descriptor.ElementsCount = elementsCount;
        descriptor.Size = elementsCount * GraphicsBackend::Current()->GetFormatSize(format);
        descriptor.Offset = offset;
        descriptor.ReadWrite = readWrite;
        return descriptor;
    }

    static GraphicsBackendBufferViewDescriptor Structured(uint32_t elementsCount, uint32_t elementSize, uint32_t offset, bool readWrite)
    {
        GraphicsBackendBufferViewDescriptor descriptor{};
        descriptor.Type = BufferType::STRUCTURED_BUFFER;
        descriptor.Format = TextureInternalFormat::INVALID;
        descriptor.ElementsCount = elementsCount;
        descriptor.Size = elementsCount * elementSize;
        descriptor.Offset = offset;
        descriptor.ReadWrite = readWrite;
        return descriptor;
    }

    static GraphicsBackendBufferViewDescriptor ByteAddress(uint32_t size, uint32_t offset, bool readWrite)
    {
        GraphicsBackendBufferViewDescriptor descriptor{};
        descriptor.Type = BufferType::BYTE_ADDRESS_BUFFER;
        descriptor.Format = TextureInternalFormat::INVALID;
        descriptor.Size = size;
        descriptor.Offset = offset;
        descriptor.ReadWrite = readWrite;
        return descriptor;
    }
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_VIEW_DESCRIPTOR_H
