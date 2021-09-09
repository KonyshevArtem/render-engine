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
    unsigned int Width;
    unsigned int Height;

    static shared_ptr<Texture> Load(const string &_path, unsigned int _width, unsigned int _height);
    static shared_ptr<Texture> White();
    static shared_ptr<Texture> ShadowMap(unsigned int _width, unsigned int _height);

    ~Texture();

private:
    void Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode);

    GLuint                m_Texture;
    GLuint                m_Sampler;
    vector<unsigned char> m_Data;

    friend class Graphics;
};

#endif //OPENGL_STUDY_TEXTURE_H
