#include "texture_2d_array.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

Texture2DArray::Texture2DArray(unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels) :
        Texture(GL_TEXTURE_2D_ARRAY, width, height, depth, mipLevels)
{
}

std::shared_ptr<Texture2DArray> Texture2DArray::ShadowMapArray(unsigned int _size, unsigned int _count)
{
    auto texture = std::shared_ptr<Texture2DArray>(new Texture2DArray(_size, _size, _count, 1));
    texture->UploadPixels(nullptr, GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, 0, 0, false);
    texture->SetWrapMode(GL_CLAMP_TO_BORDER);
    return texture;
}
