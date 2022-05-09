#include "texture.h"
#include "core_debug/debug.h"

Texture::~Texture()
{
    CHECK_GL(glDeleteTextures(1, &m_Texture));
    CHECK_GL(glDeleteSamplers(1, &m_Sampler));
}