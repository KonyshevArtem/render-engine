#ifndef OPENGL_STUDY_TEXTURE_2D_ARRAY_H
#define OPENGL_STUDY_TEXTURE_2D_ARRAY_H

#include "../texture/texture.h"
#include <memory>
#include <vector>

using namespace std;

class Texture2DArray: public Texture
{
public:
    static shared_ptr<Texture2DArray> ShadowMapArray(unsigned int _size, unsigned int _count);

    void Bind(int _unit) const override;
    void Attach(int _attachment, int _layer) const;

    inline unsigned int GetCount() const
    {
        return m_Count;
    }

    virtual ~Texture2DArray() = default;

private:
    Texture2DArray()                       = default;
    Texture2DArray(const Texture2DArray &) = delete;
    Texture2DArray(Texture2DArray &&)      = delete;

    Texture2DArray &operator=(const Texture2DArray &) = delete;
    Texture2DArray &operator=(Texture2DArray &&)      = delete;

    void Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode);

    unsigned int          m_Count;
    vector<unsigned char> m_Data;
};

#endif //OPENGL_STUDY_TEXTURE_2D_ARRAY_H
