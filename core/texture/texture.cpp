#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "texture.h"
#include "../../utils/lodepng.h"
#include "GLUT/glut.h"
#include "OpenGL/gl3.h"

using namespace std;

static shared_ptr<Texture> WhiteTexture = nullptr;

shared_ptr<Texture> Texture::Load(const string &_path, unsigned int _width, unsigned int _height)
{
    auto t    = make_shared<Texture>();
    t->Width  = _width;
    t->Height = _height;

    unsigned error = lodepng::decode(t->m_Data, _width, _height, _path, LCT_RGB);
    if (error != 0)
    {
        printf("Error loading texture: %u: %s\n", error, lodepng_error_text(error));
        return nullptr;
    }

    t->Init();

    return t;
}

shared_ptr<Texture> Texture::White()
{
    if (WhiteTexture != nullptr)
        return WhiteTexture;

    WhiteTexture         = make_shared<Texture>();
    WhiteTexture->Width  = 1;
    WhiteTexture->Height = 1;

    WhiteTexture->m_Data.push_back(255);
    WhiteTexture->m_Data.push_back(255);
    WhiteTexture->m_Data.push_back(255);

    WhiteTexture->Init();

    return WhiteTexture;
}

void Texture::Init()
{
    glGenTextures(1, &m_Texture);
    glGenSamplers(1, &m_Sampler);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, &m_Data[0]); // NOLINT(cppcoreguidelines-narrowing-conversions)
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_Texture);
    glDeleteSamplers(1, &m_Sampler);
}
