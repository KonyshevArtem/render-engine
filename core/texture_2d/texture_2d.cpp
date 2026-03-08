#include "texture_2d.h"

Texture2D::Texture2D(const GraphicsBackendTextureDescriptor & descriptor, const std::string& name) :
        Texture(TextureType::TEXTURE_2D, descriptor, name)
{
}

std::shared_ptr<Texture2D> Texture2D::Create(const GraphicsBackendTextureDescriptor& descriptor, const std::string& name)
{
    return std::shared_ptr<Texture2D>(new Texture2D(descriptor, name));
}

const std::shared_ptr<Texture2D> &Texture2D::White()
{
    static std::shared_ptr<Texture2D> white;

    if (white == nullptr)
    {
        GraphicsBackendTextureDescriptor descriptor;
        descriptor.Width = 1;
        descriptor.Height = 1;
        descriptor.MipLevels = 1;
        descriptor.Linear = false;
        descriptor.RenderTarget = false;
        descriptor.Format = TextureInternalFormat::RGBA8;

	    constexpr uint8_t pixels[4] {255, 255, 255, 255};
        white = std::shared_ptr<Texture2D>(new Texture2D(descriptor, "White"));
        white->UploadPixels(pixels, 4, 0, 0);
    }

    return white;
}

const std::shared_ptr<Texture2D> &Texture2D::Normal()
{
    static std::shared_ptr<Texture2D> normal;

    if (normal == nullptr)
    {
        GraphicsBackendTextureDescriptor descriptor;
        descriptor.Width = 1;
        descriptor.Height = 1;
        descriptor.MipLevels = 1;
        descriptor.Linear = true;
        descriptor.RenderTarget = false;
        descriptor.Format = TextureInternalFormat::RGBA8;

        constexpr uint8_t pixels[4] {125, 125, 255, 255};
        normal = std::shared_ptr<Texture2D>(new Texture2D(descriptor, "DefaultNormal"));
        normal->UploadPixels(pixels, 4, 0, 0);
    }

    return normal;
}

const std::shared_ptr<Texture2D> &Texture2D::Null()
{
    static std::shared_ptr<Texture2D> null;
    return null;
}
