#ifndef OPENGL_STUDY_TEXTURE_H
#define OPENGL_STUDY_TEXTURE_H

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "string"
#include "vector"

class Texture
{
public:
    unsigned int               width;
    unsigned int               height;
    std::vector<unsigned char> data;
    GLuint                     texture;

    static Texture *Load(const std::string &path, unsigned int width, unsigned int height);
    static Texture *White();

private:
    void Init();
};

#endif //OPENGL_STUDY_TEXTURE_H
