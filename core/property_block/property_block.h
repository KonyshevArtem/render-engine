#ifndef RENDER_ENGINE_PROPERTY_BLOCK_H
#define RENDER_ENGINE_PROPERTY_BLOCK_H

#include "matrix4x4/matrix4x4.h"
#include "vector4/vector4.h"
#include <memory>
#include <string>
#include <unordered_map>

class Texture;

class PropertyBlock
{
public:
    void SetTexture(const std::string &_name, std::shared_ptr<Texture> _value);
    void SetVector(const std::string &_name, const Vector4 &_value);
    void SetFloat(const std::string &_name, float _value);
    void SetMatrix(const std::string &_name, const Matrix4x4 &_value);
    void SetInt(const std::string &name, int value);

    const std::shared_ptr<Texture> GetTexture(const std::string &_name) const;
    Vector4                        GetVector(const std::string &_name) const;
    float                          GetFloat(const std::string &_name) const;
    Matrix4x4                      GetMatrix(const std::string &_name) const;
    int                            GetInt(const std::string &name) const;

    bool HasProperty(const std::string &name) const;

    inline const std::unordered_map<std::string, std::shared_ptr<Texture>> &GetTextures() const
    {
        return m_Textures;
    }

    inline const std::unordered_map<std::string, Vector4> &GetVectors() const
    {
        return m_Vectors;
    }

    inline const std::unordered_map<std::string, float> &GetFloats() const
    {
        return m_Floats;
    }

    inline const std::unordered_map<std::string, Matrix4x4> &GetMatrices() const
    {
        return m_Matrices;
    }

    inline const std::unordered_map<std::string, int> &GetInts() const
    {
        return m_Ints;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, Vector4> m_Vectors;
    std::unordered_map<std::string, float> m_Floats;
    std::unordered_map<std::string, Matrix4x4> m_Matrices;
    std::unordered_map<std::string, int> m_Ints;
};

#endif