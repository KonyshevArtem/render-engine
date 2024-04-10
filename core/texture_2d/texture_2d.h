#ifndef RENDER_ENGINE_TEXTURE_2D_H
#define RENDER_ENGINE_TEXTURE_2D_H

#include "texture/texture.h"
#include <filesystem>

class Texture2D: public Texture
{
public:
    static std::shared_ptr<Texture2D>        Create(int _width, int _height, TextureInternalFormat textureFormat);
    static std::shared_ptr<Texture2D>        CreateShadowMap(int _width, int _height);
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
    Texture2D(TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int mipLevels);

    static std::shared_ptr<Texture2D> Create_Internal(uint8_t *pixels, int width, int height, TextureInternalFormat textureFormat);
};

#endif //RENDER_ENGINE_TEXTURE_2D_H
