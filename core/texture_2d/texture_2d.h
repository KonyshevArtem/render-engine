#ifndef OPENGL_STUDY_TEXTURE_2D_H
#define OPENGL_STUDY_TEXTURE_2D_H

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "string"
#include "vector"
#include "filesystem"

using namespace std;

class Texture2D
{
public:
    unsigned int Width;
    unsigned int Height;

    static shared_ptr<Texture2D> Load(const filesystem::path &_path);
    static shared_ptr<Texture2D> White();

    ~Texture2D();

    void Bind(int _unit) const;

private:
    void Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode);

    GLuint                m_Texture;
    GLuint                m_Sampler;
    vector<unsigned char> m_Data;
};

#endif //OPENGL_STUDY_TEXTURE_2D_H
