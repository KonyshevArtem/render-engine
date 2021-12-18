#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "texture.h"

Texture::~Texture()
{
    glDeleteTextures(1, &m_Texture);
    glDeleteSamplers(1, &m_Sampler);
}