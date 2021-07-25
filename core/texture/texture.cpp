#define GL_SILENCE_DEPRECATION

#include "texture.h"
#include "../../utils/lodepng.h"
#include "../../utils/utils.h"
#include "GLUT/glut.h"
#include "OpenGL/gl3.h"

Texture *Texture::Load(const std::string &path, unsigned int width, unsigned int height)
{
    auto *t   = new Texture();
    t->width  = width;
    t->height = height;

    unsigned error = lodepng::decode(t->data, width, height, path, LCT_RGB);
    if (error != 0)
    {
        printf("Error loading texture: %u: %s\n", error, lodepng_error_text(error));
        return nullptr;
    }

    t->Init();

    return t;
}

Texture *Texture::White()
{
    auto *t   = new Texture();
    t->width  = 1;
    t->height = 1;

    t->data.push_back(255);
    t->data.push_back(255);
    t->data.push_back(255);

    t->Init();

    return t;
}

void Texture::Init()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
    glBindTexture(GL_TEXTURE_2D, 0);
}
