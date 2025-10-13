#ifndef RENDER_ENGINE_BUFFER_INFO_H
#define RENDER_ENGINE_BUFFER_INFO_H

#include "enums/buffer_type.h"

#include <unordered_map>
#include <string>

class GraphicsBackendBufferInfo
{
public:
    GraphicsBackendBufferInfo(uint32_t binding, uint32_t size, BufferType bufferType, std::unordered_map<std::string, int> variables) :
        m_Binding(binding),
        m_Size(size),
        m_BufferType(bufferType),
        m_Variables(std::move(variables)) {}

    ~GraphicsBackendBufferInfo() = default;

    inline uint32_t GetBinding() const
    {
        return m_Binding;
    }

    inline uint32_t GetSize() const
    {
        return m_Size;
    }

    inline BufferType GetBufferType() const
    {
        return m_BufferType;
    }

    inline const std::unordered_map<std::string, int>& GetVariables() const
    {
        return m_Variables;
    }

    GraphicsBackendBufferInfo(const GraphicsBackendBufferInfo &) = delete;
    GraphicsBackendBufferInfo(GraphicsBackendBufferInfo &&) = delete;

    GraphicsBackendBufferInfo &operator()(const GraphicsBackendBufferInfo &) = delete;
    GraphicsBackendBufferInfo &operator()(GraphicsBackendBufferInfo &&) = delete;

private:
    uint32_t m_Binding;
    uint32_t m_Size;
    BufferType m_BufferType;

    std::unordered_map<std::string, int> m_Variables;
};

#endif //RENDER_ENGINE_BUFFER_INFO_H
