#ifndef OPENGL_STUDY_MATERIAL_H
#define OPENGL_STUDY_MATERIAL_H

#include <memory>
#include <string>
#include <unordered_map>

struct Vector4;
class Texture;
class Shader;

using namespace std;

class Material
{
public:
    explicit Material(shared_ptr<Shader> _shader);
    ~Material() = default;

    void SetTexture(const string &_name, shared_ptr<Texture> _value);
    void SetVector4(const string &_name, const Vector4 &_value);
    void SetFloat(const string &_name, float _value);

    const shared_ptr<Texture> GetTexture(const string &_name) const;
    const Vector4             GetVector4(const string &_name) const;
    float                     GetFloat(const string &_name) const;

    const shared_ptr<Shader> &GetShader() const;
    void                      TransferUniforms() const;

    inline int GetRenderQueue() const
    {
        return m_RenderQueue;
    }

    inline void SetRenderQueue(int _renderQueue)
    {
        m_RenderQueue = _renderQueue;
    }

private:
    Material(const Material &) = delete;
    Material(Material &&)      = delete;

    Material &operator=(const Material &) = delete;
    Material &operator=(Material &&)      = delete;

    shared_ptr<Shader>                         m_Shader;
    unordered_map<string, shared_ptr<Texture>> m_Textures;
    unordered_map<string, Vector4>             m_Vectors4;
    unordered_map<string, float>               m_Floats;
    int                                        m_RenderQueue = 2000;
};

#endif //OPENGL_STUDY_MATERIAL_H
