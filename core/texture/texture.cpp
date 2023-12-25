#include "texture.h"
#include "debug.h"
#include "vector4/vector4.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

Texture::Texture(unsigned int target, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels) :
        m_Target(target),
        m_Width(width),
        m_Height(height),
        m_Depth(depth),
        m_MipLevels(mipLevels)
{
    CHECK_GL(glGenTextures(1, &m_Texture));
    CHECK_GL(glGenSamplers(1, &m_Sampler));
    CHECK_GL(glBindTexture(m_Target, m_Texture));
    SetWrapMode_Internal(GL_REPEAT);
    SetFiltering_Internal(mipLevels > 1 ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
    CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_MAX_LEVEL, mipLevels - 1));
    CHECK_GL(glBindTexture(m_Target, 0));
}

Texture::~Texture()
{
    CHECK_GL(glDeleteTextures(1, &m_Texture));
    CHECK_GL(glDeleteSamplers(1, &m_Sampler));
}

void Texture::Bind(int unit) const
{
    CHECK_GL(glActiveTexture(GL_TEXTURE0 + unit));
    CHECK_GL(glBindTexture(m_Target, m_Texture));
    CHECK_GL(glBindSampler(unit, m_Sampler));
}

void Texture::Attach(int attachment, int level, int layer) const
{
    if (m_Target == GL_TEXTURE_2D)
    {
        CHECK_GL(glFramebufferTexture(GL_FRAMEBUFFER, attachment, m_Texture, level));
    }
    else
    {
        CHECK_GL(glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, m_Texture, level, layer));
    }
}

void Texture::SetBaseMipLevel(unsigned int baseMipLevel) const
{
    CHECK_GL(glBindTexture(m_Target, m_Texture));
    CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_BASE_LEVEL, baseMipLevel));
    CHECK_GL(glBindTexture(m_Target, 0));
}

void Texture::SetWrapMode(int wrapMode) const
{
    CHECK_GL(glBindTexture(m_Target, m_Texture));
    SetWrapMode_Internal(wrapMode);
    CHECK_GL(glBindTexture(m_Target, 0));
}

void Texture::SetWrapMode_Internal(int wrapMode) const
{
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, wrapMode));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, wrapMode));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_R, wrapMode));
}

void Texture::SetBorderColor(const Vector4 &color) const
{
    CHECK_GL(glBindTexture(m_Target, m_Texture));
    CHECK_GL(glSamplerParameterfv(m_Sampler, GL_TEXTURE_BORDER_COLOR, &color.x));
    CHECK_GL(glBindTexture(m_Target, 0));
}

void Texture::SetFiltering_Internal(int filtering) const
{
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, filtering));

    if (filtering == GL_LINEAR_MIPMAP_NEAREST || filtering == GL_LINEAR_MIPMAP_LINEAR)
    {
        filtering = GL_LINEAR;
    }
    else if (filtering == GL_NEAREST_MIPMAP_NEAREST || filtering == GL_NEAREST_MIPMAP_LINEAR)
    {
        filtering = GL_NEAREST;
    }
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, filtering));
}

void Texture::UploadPixels(void *pixels, int uploadTarget, int internalFormat, int format, int type, int size, int mipLevel, bool compressed) const
{
    int sizeMultiplier = 1 << mipLevel;
    unsigned int width = m_Width / sizeMultiplier;
    unsigned int height = m_Height / sizeMultiplier;

    bool isImage3D = m_Target == GL_TEXTURE_2D_ARRAY;

    CHECK_GL(glBindTexture(m_Target, m_Texture));
    if (compressed)
    {
        if (isImage3D)
        {
            CHECK_GL(glCompressedTexImage3D(uploadTarget, mipLevel, internalFormat, width, height, m_Depth, 0, size, pixels)); // NOLINT(cppcoreguidelines-narrowing-conversions)
        }
        else
        {
            CHECK_GL(glCompressedTexImage2D(uploadTarget, mipLevel, internalFormat, width, height, 0, size, pixels)); // NOLINT(cppcoreguidelines-narrowing-conversions)
        }
    }
    else
    {
        if (isImage3D)
        {
            CHECK_GL(glTexImage3D(uploadTarget, mipLevel, internalFormat, width, height, m_Depth, 0, format, type, pixels));
        }
        else
        {
            CHECK_GL(glTexImage2D(uploadTarget, mipLevel, internalFormat, width, height, 0, format, type, pixels));
        }
    }
    CHECK_GL(glBindTexture(m_Target, 0));
}