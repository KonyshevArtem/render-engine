#include "texture_2d_array.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

std::shared_ptr<Texture2DArray> Texture2DArray::ShadowMapArray(unsigned int _size, unsigned int _count)
{
    auto texture      = std::shared_ptr<Texture2DArray>(new Texture2DArray());
    texture->m_Width  = _size;
    texture->m_Height = _size;
    texture->m_Count  = _count;

    texture->Init(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_BORDER);

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
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _internalFormat, m_Width, m_Height, m_Count, 0, _format, _type, nullptr); // NOLINT(cppcoreguidelines-narrowing-conversions)
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
