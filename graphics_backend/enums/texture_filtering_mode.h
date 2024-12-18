#ifndef RENDER_ENGINE_TEXTURE_FILTERING_MODE_H
#define RENDER_ENGINE_TEXTURE_FILTERING_MODE_H

enum class TextureFilteringMode
{
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR,
};

#endif //RENDER_ENGINE_TEXTURE_FILTERING_MODE_H
