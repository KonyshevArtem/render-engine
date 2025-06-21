#include "graphics_buffer_wrapper.h"
#include "graphics_buffer.h"
#include "shader/shader.h"
#include "types/graphics_backend_buffer_info.h"

GraphicsBufferWrapper::GraphicsBufferWrapper(const std::shared_ptr<Shader>& shader, const std::string& bufferName, const std::string& debugName)
{
    const auto &buffers = shader->GetBuffers();

    auto it = buffers.find(bufferName);
    if (it != buffers.end())
    {
        std::string name = debugName;
        name.append("_");
        name.append(bufferName);

        const auto &bufferInfo = it->second;
        m_Buffer = std::make_shared<GraphicsBuffer>(bufferInfo->GetSize(), name, false);
        m_BufferInfo = bufferInfo;
    }
    else
    {
        m_Buffer = nullptr;
        m_BufferInfo = nullptr;
    }
}

void GraphicsBufferWrapper::TrySetVariable(const std::string &variableName, const void *data, uint64_t size)
{
    if (m_BufferInfo)
    {
        const auto &bufferVariables = m_BufferInfo->GetVariables();
        auto it = bufferVariables.find(variableName);
        if (it != bufferVariables.end())
        {
            m_Buffer->SetData(data, it->second, size);
        }
    }
}
