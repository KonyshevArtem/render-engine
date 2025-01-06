#include "graphics_buffer_wrapper.h"
#include "graphics_buffer.h"
#include "shader/shader.h"
#include "shader/shader_pass/shader_pass.h"
#include "types/graphics_backend_buffer_info.h"

GraphicsBufferWrapper::GraphicsBufferWrapper(const std::shared_ptr<Shader>& shader, const std::string& bufferName, const std::string& debugName)
{
    int passesCount = shader->PassesCount();
    m_PassBuffers.resize(passesCount);
    m_PassBuffersInfo.resize(passesCount);

    for (int i = 0; i < passesCount; ++i)
    {
        const auto &shaderPass = shader->GetPass(i);
        const auto &buffers = shaderPass->GetBuffers();

        auto it = buffers.find(bufferName);
        if (it != buffers.end())
        {
            std::string name = debugName;
            name.append("_");
            name.append(bufferName);

            const auto &bufferInfo = it->second;
            m_PassBuffers[i] = std::make_shared<GraphicsBuffer>(bufferInfo->GetSize(), name, false);
            m_PassBuffersInfo[i] = bufferInfo;
        }
        else
        {
            m_PassBuffers[i] = nullptr;
            m_PassBuffersInfo[i] = nullptr;
        }
    }
}

void GraphicsBufferWrapper::TrySetVariable(const std::string &variableName, const void *data, uint64_t size)
{
    for (int i = 0; i < m_PassBuffersInfo.size(); ++i)
    {
        const auto &bufferInfo = m_PassBuffersInfo[i];
        if (bufferInfo)
        {
            const auto &bufferVariables = bufferInfo->GetVariables();
            auto it = bufferVariables.find(variableName);
            if (it != bufferVariables.end())
            {
                m_PassBuffers[i]->SetData(data, it->second, size);
            }
        }
    }
}
