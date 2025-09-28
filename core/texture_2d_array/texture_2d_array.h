#ifndef RENDER_ENGINE_TEXTURE_2D_ARRAY_H
#define RENDER_ENGINE_TEXTURE_2D_ARRAY_H

#include "texture/texture.h"
#include <memory>

class Texture2DArray: public Texture
{
public:
    static std::shared_ptr<Texture2DArray> Create(uint32_t width, uint32_t height, uint32_t slices, TextureInternalFormat format, bool isLinear, const std::string& name);

    ~Texture2DArray() override = default;

    Texture2DArray(const Texture2DArray &) = delete;
    Texture2DArray(Texture2DArray &&)      = delete;

    Texture2DArray &operator=(const Texture2DArray &) = delete;
    Texture2DArray &operator=(Texture2DArray &&)      = delete;

private:
    Texture2DArray(TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, bool isLinear, const std::string& name);
};

#endif //RENDER_ENGINE_TEXTURE_2D_ARRAY_H
