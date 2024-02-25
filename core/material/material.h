#ifndef OPENGL_STUDY_MATERIAL_H
#define OPENGL_STUDY_MATERIAL_H

#include "matrix4x4/matrix4x4.h"
#include "property_block/property_block.h"
#include "vector4/vector4.h"
#include "shader/uniform_info/uniform_info.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

class Texture;
class Shader;
class GraphicsBuffer;
class GraphicsBufferWrapper;

class Material
{
public:
    explicit Material(std::shared_ptr<Shader> _shader);
    ~Material() = default;

    Material(const Material &) = delete;
    Material(Material &&) = delete;

    Material &operator=(const Material &) = delete;
    Material &operator=(Material &&) = delete;

    std::shared_ptr<GraphicsBuffer> GetPerMaterialDataBlock(int pass) const;

    void SetTexture(const std::string &name, std::shared_ptr<Texture> texture);
    inline const std::shared_ptr<Texture> GetTexture(const std::string &_name) const
    {
        return m_PropertyBlock.GetTexture(_name);
    }


    void SetVector(const std::string &_name, const Vector4 &_value);
    inline Vector4 GetVector(const std::string &_name) const
    {
        return m_PropertyBlock.GetVector(_name);
    }


    void SetFloat(const std::string &_name, float _value);
    inline float GetFloat(const std::string &_name) const
    {
        return m_PropertyBlock.GetFloat(_name);
    }


    void SetMatrix(const std::string &_name, const Matrix4x4 &_value);
    inline Matrix4x4 GetMatrix(const std::string &_name) const
    {
        return m_PropertyBlock.GetMatrix(_name);
    }


    inline const std::shared_ptr<Shader> &GetShader() const
    {
        return m_Shader;
    }

    inline const PropertyBlock &GetPropertyBlock() const
    {
        return m_PropertyBlock;
    }


    inline int GetRenderQueue() const
    {
        return m_RenderQueue;
    }

    inline void SetRenderQueue(int _renderQueue)
    {
        m_RenderQueue = _renderQueue;
    }

private:
    void SetDataToUniformBlocks(const std::string &name, const void *data, uint64_t size);

    std::shared_ptr<GraphicsBufferWrapper> m_PerMaterialDataBufferWrapper;
    std::shared_ptr<Shader> m_Shader;
    PropertyBlock m_PropertyBlock;
    int m_RenderQueue = 2000;
};

#endif //OPENGL_STUDY_MATERIAL_H
