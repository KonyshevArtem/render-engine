#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "cubemap.h"
#include "../../external/lodepng/lodepng.h"
#include "../../utils/utils.h"

shared_ptr<Cubemap> Cubemap::Load(const filesystem::path &_xPositivePath,
                                  const filesystem::path &_xNegativePath,
                                  const filesystem::path &_yPositivePath,
                                  const filesystem::path &_yNegativePath,
                                  const filesystem::path &_zPositivePath,
                                  const filesystem::path &_zNegativePath)
{
    auto cubemap = make_shared<Cubemap>();

    cubemap->m_Data.resize(6);
    vector<filesystem::path> paths = {_xPositivePath, _xNegativePath, _yPositivePath, _yNegativePath, _zPositivePath, _zNegativePath};
    for (int i = 0; i < 6; ++i)
    {
        unsigned int width  = 0;
        unsigned int height = 0;
        unsigned     error  = lodepng::decode(cubemap->m_Data[i], width, height, Utils::GetExecutableDirectory() / paths[i], LCT_RGB);
        if (error != 0)
        {
            printf("Error loading texture: %u: %s\n", error, lodepng_error_text(error));
            return nullptr;
        }

        cubemap->Width  = width;
        cubemap->Height = height;
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
    for (int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, &m_Data[i][0]); // NOLINT(cppcoreguidelines-narrowing-conversions)
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
    if (m_White != nullptr)
        return m_White;

    m_White         = make_shared<Cubemap>();
    m_White->Width  = 1;
    m_White->Height = 1;

    m_White->m_Data.resize(6);
    for (int i = 0; i < 6; ++i)
    {
        m_White->m_Data[i].push_back(255);
        m_White->m_Data[i].push_back(255);
        m_White->m_Data[i].push_back(255);
    }

    m_White->Init();
    return m_White;
}
