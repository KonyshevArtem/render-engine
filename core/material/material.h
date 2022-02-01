#ifndef OPENGL_STUDY_MATERIAL_H
#define OPENGL_STUDY_MATERIAL_H

#include <memory>
#include <string>
#include <unordered_map>

struct Vector4;
class Texture;
class Shader;

class Material
{
public:
    explicit Material(std::shared_ptr<Shader> _shader);
    ~Material() = default;

    void SetTexture(const std::string &_name, std::shared_ptr<Texture> _value);
    void SetVector4(const std::string &_name, const Vector4 &_value);
    void SetFloat(const std::string &_name, float _value);

    const std::shared_ptr<Texture> GetTexture(const std::string &_name) const;
    const Vector4                  GetVector4(const std::string &_name) const;
    float                          GetFloat(const std::string &_name) const;

    const std::shared_ptr<Shader> &GetShader() const;
    void                           TransferUniforms() const;

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

    std::shared_ptr<Shader>                                   m_Shader;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, Vector4>                  m_Vectors4;
    std::unordered_map<std::string, float>                    m_Floats;
    int                                                       m_RenderQueue = 2000;
};

#endif //OPENGL_STUDY_MATERIAL_H
