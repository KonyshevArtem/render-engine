#ifndef OPENGL_STUDY_TEXTURE_2D_H
#define OPENGL_STUDY_TEXTURE_2D_H

#include "../texture/texture.h"
#include <filesystem>
#include <string>
#include <vector>

using namespace std;

class Texture2D: public Texture
{
public:
    static shared_ptr<Texture2D>        Load(const filesystem::path &_path, bool _srgb = true, bool _hasAlpha = false);
    static const shared_ptr<Texture2D> &White();
    static const shared_ptr<Texture2D> &Normal();

    void Bind(int _unit) const override;

    static inline const shared_ptr<Texture2D> &Null()
    {
        return m_Null;
    }

private:
    Texture2D()                  = default;
    Texture2D(const Texture2D &) = delete;

    void Init(GLint _internalFormat, GLenum _format, GLenum _type, GLint _wrapMode);

    vector<unsigned char> m_Data;

    static shared_ptr<Texture2D> m_Null;
    static shared_ptr<Texture2D> m_White;
    static shared_ptr<Texture2D> m_Normal;
};

#endif //OPENGL_STUDY_TEXTURE_2D_H
