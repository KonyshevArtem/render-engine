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

    unsigned error = lodepng::decode(t->Data, _width, _height, _path, LCT_RGB);
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

    WhiteTexture->Data.push_back(255);
    WhiteTexture->Data.push_back(255);
    WhiteTexture->Data.push_back(255);

    WhiteTexture->Init();

    return WhiteTexture;
}

void Texture::Init()
{
    glGenTextures(1, &Ptr);
    glGenSamplers(1, &Sampler);
    glBindTexture(GL_TEXTURE_2D, Ptr);
    glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, &Data[0]); // NOLINT(cppcoreguidelines-narrowing-conversions)
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &Ptr);
    glDeleteSamplers(1, &Sampler);
}
