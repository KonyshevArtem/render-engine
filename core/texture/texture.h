#ifndef OPENGL_STUDY_TEXTURE_H
#define OPENGL_STUDY_TEXTURE_H

struct Vector4;

class Texture
{
public:
    virtual ~Texture();

    void Bind(int unit) const;
    void Attach(int attachment, int level, int layer) const;
    void SetBaseMipLevel(unsigned int baseMipLevel) const;
    void SetWrapMode(int wrapMode) const;
    void SetBorderColor(const Vector4 &color) const;

    inline unsigned int GetWidth() const
    {
        return m_Width;
    }

    inline unsigned int GetHeight() const
    {
        return m_Height;
    }

    Texture(const Texture &) = delete;
    Texture(Texture &&) = delete;

    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&) = delete;

protected:
    Texture(unsigned int target, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels);

    void UploadPixels(void *pixels, int uploadTarget, int internalFormat, int format, int type, int size, int mipLevel, bool compressed) const;

private:
    void SetWrapMode_Internal(int wrapMode) const;
    void SetFiltering_Internal(int filtering) const;

    unsigned int m_Width   = 0;
    unsigned int m_Height  = 0;
    unsigned int m_Depth   = 0;
    unsigned int m_Texture = 0;
    unsigned int m_Sampler = 0;
    unsigned int m_Target  = 0;
};

#endif //OPENGL_STUDY_TEXTURE_H
