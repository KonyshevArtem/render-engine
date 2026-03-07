#ifndef RENDER_ENGINE_CUBEMAP_H
#define RENDER_ENGINE_CUBEMAP_H

#include "texture/texture.h"

class Cubemap: public Texture
{
public:
    static std::shared_ptr<Cubemap> &White();
    static std::shared_ptr<Cubemap> &Black();

    ~Cubemap() override = default;

    Cubemap(const Cubemap &) = delete;
    Cubemap(Cubemap &&)      = delete;

    Cubemap &operator=(const Cubemap &) = delete;
    Cubemap &operator=(Cubemap &&) = delete;

private:
    Cubemap(const Descriptor& descriptor, const std::string& name);

    static std::shared_ptr<Cubemap> CreateDefaultCubemap(uint8_t* pixels, uint8_t size, const std::string& name);

    friend class Resources;
};

#endif