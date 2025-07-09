#include "graphics_buffer_wrapper.h"
#include "graphics_buffer.h"
#include "shader/shader.h"
#include "types/graphics_backend_buffer_info.h"
#include <cassert>

GraphicsBufferWrapper::GraphicsBufferWrapper(const std::shared_ptr<Shader>& shader, const std::string& bufferName, const std::string& debugName) :
    m_Name(debugName)
{
    const auto &buffers = shader->GetBuffers();

    auto it = buffers.find(bufferName);
    if (it != buffers.end())
    {
        std::string name = m_Name;
        name.append("_");
        name.append(bufferName);

        const auto &bufferInfo = it->second;
        m_Buffer = std::make_shared<GraphicsBuffer>(bufferInfo->GetSize(), name, false);
        m_BufferInfo = bufferInfo;
        m_Data.resize(bufferInfo->GetSize());
    }
    else
    {
        m_Buffer = nullptr;
        m_BufferInfo = nullptr;
    }
}

std::shared_ptr<GraphicsBufferWrapper> GraphicsBufferWrapper::Copy()
{
    std::shared_ptr<GraphicsBufferWrapper> bufferWrapper = std::shared_ptr<GraphicsBufferWrapper>(new GraphicsBufferWrapper());
    if (m_Buffer)
    {
        bufferWrapper->m_BufferInfo = m_BufferInfo;
        bufferWrapper->m_Name = m_Name;

        bufferWrapper->m_Data.resize(m_BufferInfo->GetSize());
        memcpy(&bufferWrapper->m_Data[0], &m_Data[0], m_BufferInfo->GetSize());

        bufferWrapper->m_Buffer = std::make_shared<GraphicsBuffer>(m_BufferInfo->GetSize(), m_Name, false);
        bufferWrapper->m_Buffer->SetData(&m_Data[0], 0, m_BufferInfo->GetSize());
    }

    return bufferWrapper;
}

void GraphicsBufferWrapper::TrySetVariable(const std::string &variableName, const void *data, uint64_t size)
{
    if (m_BufferInfo)
    {
        const auto &bufferVariables = m_BufferInfo->GetVariables();
        auto it = bufferVariables.find(variableName);
        if (it != bufferVariables.end())
        {
            assert(it->second + size <= m_Data.size());
            m_Buffer->SetData(data, it->second, size);
            memcpy(&m_Data[it->second], data, size);
        }
    }
}
