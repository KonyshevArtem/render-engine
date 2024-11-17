#ifndef RENDER_ENGINE_TEXTURE_2D_H
#define RENDER_ENGINE_TEXTURE_2D_H

#include "texture/texture.h"
#include <filesystem>

class Texture2D: public Texture
{
public:
    static std::shared_ptr<Texture2D>        Create(uint32_t _width, uint32_t _height, TextureInternalFormat textureFormat, bool isLinear, bool isRenderTarget, const std::string& name);
    static std::shared_ptr<Texture2D>        Load(const std::filesystem::path &_path);
    static const std::shared_ptr<Texture2D> &White();
    static const std::shared_ptr<Texture2D> &Normal();
    static const std::shared_ptr<Texture2D> &Null();

    ~Texture2D() override = default;

    Texture2D(const Texture2D &) = delete;
    Texture2D(Texture2D &&)      = delete;

    Texture2D &operator=(const Texture2D &) = delete;
    Texture2D &operator=(Texture2D &&) = delete;

private:
    Texture2D(TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, bool isLinear, bool isRenderTarget, const std::string& name);

    static std::shared_ptr<Texture2D> Create_Internal(uint8_t *pixels, uint32_t width, uint32_t height, TextureInternalFormat textureFormat, bool isLinear, bool isRenderTarget, const std::string& name);
};

#endif //RENDER_ENGINE_TEXTURE_2D_H
