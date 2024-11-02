#ifndef RENDER_ENGINE_TEXTURE_2D_ARRAY_H
#define RENDER_ENGINE_TEXTURE_2D_ARRAY_H

#include "texture/texture.h"
#include <memory>

class Texture2DArray: public Texture
{
public:
    static std::shared_ptr<Texture2DArray> ShadowMapArray(unsigned int _size, unsigned int _count);

    ~Texture2DArray() override = default;

    Texture2DArray(const Texture2DArray &) = delete;
    Texture2DArray(Texture2DArray &&)      = delete;

    Texture2DArray &operator=(const Texture2DArray &) = delete;
    Texture2DArray &operator=(Texture2DArray &&)      = delete;

private:
    Texture2DArray(TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels, bool isLinear);
};

#endif //RENDER_ENGINE_TEXTURE_2D_ARRAY_H
