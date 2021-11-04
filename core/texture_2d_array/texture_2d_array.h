#ifndef OPENGL_STUDY_TEXTURE_2D_ARRAY_H
#define OPENGL_STUDY_TEXTURE_2D_ARRAY_H

#include "../texture/texture.h"
#include <OpenGL/gl3.h>
#include <memory>
#include <vector>

using namespace std;

class Texture2DArray: public Texture
{
public:
    unsigned int Count;

    static shared_ptr<Texture2DArray> ShadowMapArray(unsigned int _size, unsigned int _count);

    ~Texture2DArray();

    void Bind(int _unit) const override;
    void Attach(int _attachment, int _layer) const;

private:
    void Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode);

    vector<unsigned char> m_Data;
};

#endif //OPENGL_STUDY_TEXTURE_2D_ARRAY_H
