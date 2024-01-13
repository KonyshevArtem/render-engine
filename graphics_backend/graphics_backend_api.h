#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_API_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_API_H

#include "graphics_backend.h"
#include "enums/texture_type.h"
#include "enums/texture_parameter.h"
#include "enums/texture_target.h"
#include "enums/texture_level_parameter.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_pixel_format.h"
#include "enums/texture_data_type.h"

#ifdef OPENGL_STUDY_EDITOR
#define CHECK_GRAPHICS_BACKEND_FUNC(backendFunction)                   \
    backendFunction;                                                   \
    {                                                                  \
        auto error = GraphicsBackend::GetError();                      \
        if (error != 0)                                                \
            GraphicsBackendDebug::LogError(error, __FILE__, __LINE__); \
    }
#else
#define CHECK_GRAPHICS_BACKEND_FUNC(backendFunction) backendFunction;
#endif

namespace GraphicsBackend
{
    void GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr);
    void DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr);
    void BindTexture(TextureType type, GraphicsBackendTexture texture);
    void GenerateMipmaps(TextureType type);
    void SetTextureParameterInt(TextureType type, TextureParameter parameter, int value);
    void GetTextureLevelParameterInt(TextureTarget target, GraphicsBackendTextureLevel level, TextureLevelParameter parameter, int* outValues);
    void TextureImage2D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, void* pixelsData);
    void GetTextureImage(TextureTarget target, GraphicsBackendTextureLevel level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels);
    void GetCompressedTextureImage(TextureTarget target, GraphicsBackendTextureLevel level, void* outPixels);

    GraphicsBackendError GetError();
    const char *GetErrorString(GraphicsBackendError error);
}

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_API_H
