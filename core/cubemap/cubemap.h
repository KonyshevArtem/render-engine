#ifndef OPENGL_STUDY_CUBEMAP_H
#define OPENGL_STUDY_CUBEMAP_H

#include "texture/texture.h"
#include <filesystem>
#include <memory>
#include <vector>

class Cubemap: public Texture
{
public:
    static std::shared_ptr<Cubemap> Load(const std::filesystem::path &_xPositivePath,
                                         const std::filesystem::path &_xNegativePath,
                                         const std::filesystem::path &_yPositivePath,
                                         const std::filesystem::path &_yNegativePath,
                                         const std::filesystem::path &_zPositivePath,
                                         const std::filesystem::path &_zNegativePath);

    void                        Bind(int _unit) const override;
    static std::shared_ptr<Cubemap> &White();

    ~Cubemap() = default;

private:
    Cubemap()                = default;
    Cubemap(const Cubemap &) = delete;
    Cubemap(Cubemap &&)      = delete;

    Cubemap &operator=(const Cubemap &) = delete;
    Cubemap &operator=(Cubemap &&)      = delete;

    void Init();

    std::vector<std::vector<unsigned char>> m_Data;

    static constexpr unsigned int SIDES_COUNT = 6;
};

#endif