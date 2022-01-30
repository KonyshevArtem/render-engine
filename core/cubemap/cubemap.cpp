#include "cubemap.h"
#include "../../external/lodepng/lodepng.h"
#include "../../utils/utils.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

shared_ptr<Cubemap> Cubemap::Load(const filesystem::path &_xPositivePath,
                                  const filesystem::path &_xNegativePath,
                                  const filesystem::path &_yPositivePath,
                                  const filesystem::path &_yNegativePath,
                                  const filesystem::path &_zPositivePath,
                                  const filesystem::path &_zNegativePath)
{
    auto cubemap = shared_ptr<Cubemap>(new Cubemap());

    cubemap->m_Data.resize(SIDES_COUNT);
    filesystem::path paths[SIDES_COUNT] {_xPositivePath, _xNegativePath, _yPositivePath, _yNegativePath, _zPositivePath, _zNegativePath};

    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        auto error = lodepng::decode(cubemap->m_Data[i], cubemap->m_Width, cubemap->m_Height, (Utils::GetExecutableDirectory() / paths[i]).string(), LCT_RGB);
        if (error != 0)
        {
            printf("Error loading texture: %u: %s\n", error, lodepng_error_text(error));
            return nullptr;
        }
    }

    cubemap->Init();
    return cubemap;
}

void Cubemap::Init()
{
    glGenTextures(1, &m_Texture);
    glGenSamplers(1, &m_Sampler);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    for (int i = 0; i < SIDES_COUNT; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, &m_Data[i][0]); // NOLINT(cppcoreguidelines-narrowing-conversions)
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::Bind(int _unit) const
{
    glActiveTexture(GL_TEXTURE0 + _unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);
    glBindSampler(_unit, m_Sampler);
}

shared_ptr<Cubemap> &Cubemap::White()
{
    static shared_ptr<Cubemap> white;

    if (white != nullptr)
        return white;

    white           = shared_ptr<Cubemap>(new Cubemap());
    white->m_Width  = 1;
    white->m_Height = 1;

    white->m_Data.resize(SIDES_COUNT);
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        white->m_Data[i].push_back(255);
        white->m_Data[i].push_back(255);
        white->m_Data[i].push_back(255);
    }

    white->Init();
    return white;
}
