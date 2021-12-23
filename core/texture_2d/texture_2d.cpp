#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "texture_2d.h"
#include "../../external/lodepng/lodepng.h"
#include "../../utils/utils.h"
#include <OpenGL/gl3.h>

using namespace std;

shared_ptr<Texture2D> Texture2D::Load(const filesystem::path &_path, bool _srgb, bool _hasAlpha)
{
    auto t = shared_ptr<Texture2D>(new Texture2D());

    auto colorType = _hasAlpha ? LCT_RGBA : LCT_RGB;
    auto error     = lodepng::decode(t->m_Data, t->m_Width, t->m_Height, Utils::GetExecutableDirectory() / _path, colorType);
    if (error != 0)
    {
        printf("Error loading texture: %u: %s\n", error, lodepng_error_text(error));
        return nullptr;
    }

    auto internalFormat = _srgb
                                  ? _hasAlpha
                                            ? GL_SRGB_ALPHA
                                            : GL_SRGB
                          : _hasAlpha
                                  ? GL_RGBA
                                  : GL_RGB;

    auto format = _hasAlpha ? GL_RGBA : GL_RGB;

    t->Init(internalFormat, format, GL_UNSIGNED_BYTE, GL_REPEAT);

    return t;
}

const shared_ptr<Texture2D> &Texture2D::White()
{
    static shared_ptr<Texture2D> white;

    if (white != nullptr)
        return white;

    white           = shared_ptr<Texture2D>(new Texture2D());
    white->m_Width  = 1;
    white->m_Height = 1;

    white->m_Data.push_back(255);
    white->m_Data.push_back(255);
    white->m_Data.push_back(255);

    white->Init(GL_SRGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT);

    return white;
}

const shared_ptr<Texture2D> &Texture2D::Normal()
{
    static shared_ptr<Texture2D> normal;

    if (normal != nullptr)
        return normal;

    normal           = shared_ptr<Texture2D>(new Texture2D());
    normal->m_Width  = 1;
    normal->m_Height = 1;

    normal->m_Data.push_back(125);
    normal->m_Data.push_back(125);
    normal->m_Data.push_back(255);

    normal->Init(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT);

    return normal;
}

const shared_ptr<Texture2D> &Texture2D::Null()
{
    static shared_ptr<Texture2D> null;
    return null;
}

void Texture2D::Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode)
{
    glGenTextures(1, &m_Texture);
    glGenSamplers(1, &m_Sampler);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, _wrapMode);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, _wrapMode);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, m_Width, m_Height, 0, _format, _type, &m_Data[0]); // NOLINT(cppcoreguidelines-narrowing-conversions)
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind(int _unit) const
{
    glActiveTexture(GL_TEXTURE0 + _unit);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glBindSampler(_unit, m_Sampler);
}
