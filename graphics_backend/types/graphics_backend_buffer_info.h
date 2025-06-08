#ifndef RENDER_ENGINE_BUFFER_INFO_H
#define RENDER_ENGINE_BUFFER_INFO_H

#include "types/graphics_backend_resource_bindings.h"

#include <unordered_map>
#include <string>

class GraphicsBackendBufferInfo
{
public:
    GraphicsBackendBufferInfo(int size, bool isConstant, std::unordered_map<std::string, int> variables) : m_Size(size), m_IsConstant(isConstant), m_Variables(std::move(variables)) {}
    ~GraphicsBackendBufferInfo() = default;

    inline GraphicsBackendResourceBindings GetBinding() const
    {
        return m_Binding;
    }

    inline void SetBindings(GraphicsBackendResourceBindings bindings)
    {
        m_Binding = bindings;
    }

    inline int GetSize() const
    {
        return m_Size;
    }

    inline bool IsConstant() const
    {
        return m_IsConstant;
    }

    inline const std::unordered_map<std::string, int> &GetVariables() const
    {
        return m_Variables;
    }

    GraphicsBackendBufferInfo(const GraphicsBackendBufferInfo &) = delete;
    GraphicsBackendBufferInfo(GraphicsBackendBufferInfo &&) = delete;

    GraphicsBackendBufferInfo &operator()(const GraphicsBackendBufferInfo &) = delete;
    GraphicsBackendBufferInfo &operator()(GraphicsBackendBufferInfo &&) = delete;

private:
    GraphicsBackendResourceBindings m_Binding;
    int m_Size;
    bool m_IsConstant;

    std::unordered_map<std::string, int> m_Variables;
};

#endif //RENDER_ENGINE_BUFFER_INFO_H
