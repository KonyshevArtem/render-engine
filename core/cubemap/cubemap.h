#ifndef OPENGL_STUDY_CUBEMAP_H
#define OPENGL_STUDY_CUBEMAP_H

#include "texture/texture.h"
#include <filesystem>

class Cubemap: public Texture
{
public:
    static std::shared_ptr<Cubemap> Load(const std::filesystem::path &path);

    static std::shared_ptr<Cubemap> &White();
    static std::shared_ptr<Cubemap> &Black();

    ~Cubemap() override = default;

    Cubemap(const Cubemap &) = delete;
    Cubemap(Cubemap &&)      = delete;

    Cubemap &operator=(const Cubemap &) = delete;
    Cubemap &operator=(Cubemap &&) = delete;

private:
    Cubemap(unsigned int width, unsigned int height, unsigned int mipLevels);

    static std::shared_ptr<Cubemap> CreateDefaultCubemap(unsigned char *pixels);

    static constexpr unsigned int SIDES_COUNT = 6;
};

#endif