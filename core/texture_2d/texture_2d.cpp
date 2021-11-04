#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "texture_2d.h"
#include "../../external/lodepng/lodepng.h"
#include "../../utils/utils.h"
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>

using namespace std;

shared_ptr<Texture2D> Texture2D::m_White  = nullptr;
shared_ptr<Texture2D> Texture2D::m_Normal = nullptr;

shared_ptr<Texture2D> Texture2D::Load(const filesystem::path &_path, bool _srgb)
{
    auto t = make_shared<Texture2D>();

    unsigned int width  = 0;
    unsigned int height = 0;
    unsigned     error  = lodepng::decode(t->m_Data, width, height, Utils::GetExecutableDirectory() / _path, LCT_RGB);
    if (error != 0)
    {
        printf("Error loading texture: %u: %s\n", error, lodepng_error_text(error));
        return nullptr;
    }

    t->Width  = width;
    t->Height = height;
    t->Init(_srgb ? GL_SRGB : GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT);

    return t;
}

shared_ptr<Texture2D> Texture2D::White()
{
    if (m_White != nullptr)
        return m_White;

    m_White         = make_shared<Texture2D>();
    m_White->Width  = 1;
    m_White->Height = 1;

    m_White->m_Data.push_back(255);
    m_White->m_Data.push_back(255);
    m_White->m_Data.push_back(255);

    m_White->Init(GL_SRGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT);

    return m_White;
}

shared_ptr<Texture2D> Texture2D::Normal()
{
    if (m_Normal != nullptr)
        return m_Normal;
    
    m_Normal         = make_shared<Texture2D>();
    m_Normal->Width  = 1;
    m_Normal->Height = 1;

    m_Normal->m_Data.push_back(125);
    m_Normal->m_Data.push_back(125);
    m_Normal->m_Data.push_back(255);

    m_Normal->Init(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT);

    return m_Normal;
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
    glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, Width, Height, 0, _format, _type, &m_Data[0]); // NOLINT(cppcoreguidelines-narrowing-conversions)
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_Texture);
    glDeleteSamplers(1, &m_Sampler);
}

void Texture2D::Bind(int _unit) const
{
    glActiveTexture(GL_TEXTURE0 + _unit);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glBindSampler(_unit, m_Sampler);
}
