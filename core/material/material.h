#ifndef RENDER_ENGINE_MATERIAL_H
#define RENDER_ENGINE_MATERIAL_H

#include "matrix4x4/matrix4x4.h"
#include "vector4/vector4.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_stencil_descriptor.h"
#include "resources/resource.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

class Texture;
class Shader;
class GraphicsBuffer;
class GraphicsBufferWrapper;

class Material : public Resource
{
public:
    Material(std::shared_ptr<Shader> shader, const std::string& name);
    ~Material() = default;

    Material(const Material &) = delete;
    Material(Material &&) = delete;

    Material &operator=(const Material &) = delete;
    Material &operator=(Material &&) = delete;

    std::shared_ptr<Material> Copy();
    std::shared_ptr<GraphicsBuffer> GetPerMaterialDataBuffer(uint32_t& bindingIndex) const;

    void SetTexture(const std::string &name, std::shared_ptr<Texture> texture);
    void SetVector(const std::string &_name, const Vector4 &_value);
    void SetFloat(const std::string &_name, float _value);
    void SetMatrix(const std::string &_name, const Matrix4x4 &_value);
    void SetInt(const std::string &name, int value);

    inline const std::shared_ptr<Shader>& GetShader() const
    {
        return m_Shader;
    }

    inline const std::unordered_map<uint32_t, std::shared_ptr<Texture>>& GetTextures() const
    {
        return m_Textures;
    }

    inline int GetRenderQueue() const
    {
        return m_RenderQueue;
    }

    inline void SetRenderQueue(int _renderQueue)
    {
        m_RenderQueue = _renderQueue;
    }

    GraphicsBackendStencilDescriptor StencilDescriptor{};

private:
    Material() = default;

    void SetDataToConstantBuffer(const std::string &name, const void *data, uint64_t size);

    std::shared_ptr<GraphicsBufferWrapper> m_PerMaterialDataBufferWrapper;
    std::shared_ptr<Shader> m_Shader;
    std::unordered_map<uint32_t, std::shared_ptr<Texture>> m_Textures;
    std::string m_Name;
    int m_RenderQueue = 2000;
};

#endif //RENDER_ENGINE_MATERIAL_H
