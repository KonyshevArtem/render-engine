#ifndef OPENGL_STUDY_TEXTURE_H
#define OPENGL_STUDY_TEXTURE_H

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "string"
#include "vector"

using namespace std;

class Texture
{
public:
    unsigned int          Width;
    unsigned int          Height;
    vector<unsigned char> Data;
    GLuint                Ptr;
    GLuint                Sampler;

    static shared_ptr<Texture> Load(const string &path, unsigned int width, unsigned int height);
    static shared_ptr<Texture> White();

private:
    void Init();
};

#endif //OPENGL_STUDY_TEXTURE_H
