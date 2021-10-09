#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "texture_2d_array.h"

Texture2DArray::~Texture2DArray()
{
    glDeleteTextures(1, &m_Texture);
    glDeleteSamplers(1, &m_Sampler);
}

shared_ptr<Texture2DArray> Texture2DArray::ShadowMapArray(unsigned int _size, unsigned int _count)
{
    auto texture    = make_shared<Texture2DArray>();
    texture->Width  = _size;
    texture->Height = _size;
    texture->Count  = _count;

    texture->Init(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_BORDER);
    glSamplerParameterf(texture->m_Sampler, GL_TEXTURE_BORDER_COLOR, 1.0f);

    return texture;
}

void Texture2DArray::Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode)
{
    glGenTextures(1, &m_Texture);
    glGenSamplers(1, &m_Sampler);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_Texture);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, _wrapMode);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, _wrapMode);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_R, _wrapMode);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _internalFormat, Width, Height, Count, 0, _format, _type, &m_Data[0]); // NOLINT(cppcoreguidelines-narrowing-conversions)
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Texture2DArray::Bind(int _unit) const
{
    glActiveTexture(GL_TEXTURE0 + _unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_Texture);
    glBindSampler(_unit, m_Sampler);
}

void Texture2DArray::Attach(int _attachment, int _layer) const
{
    glFramebufferTextureLayer(GL_FRAMEBUFFER, _attachment, m_Texture, 0, _layer);
}
