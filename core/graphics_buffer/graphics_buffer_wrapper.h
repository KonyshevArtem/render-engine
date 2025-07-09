#ifndef RENDER_ENGINE_GRAPHICS_BUFFER_WRAPPER_H
#define RENDER_ENGINE_GRAPHICS_BUFFER_WRAPPER_H

#include <memory>
#include <string>
#include <vector>

class Shader;
class GraphicsBuffer;
class GraphicsBackendBufferInfo;

class GraphicsBufferWrapper
{
public:
    GraphicsBufferWrapper(const std::shared_ptr<Shader>& shader, const std::string& bufferName, const std::string& debugName);
    ~GraphicsBufferWrapper() = default;

    std::shared_ptr<GraphicsBufferWrapper> Copy();

    void TrySetVariable(const std::string &variableName, const void *data, uint64_t size);

    inline const std::shared_ptr<GraphicsBuffer>& GetBuffer() const
    {
        return m_Buffer;
    }

    GraphicsBufferWrapper(const GraphicsBufferWrapper &) = delete;
    GraphicsBufferWrapper(GraphicsBufferWrapper &&) = delete;

    GraphicsBufferWrapper &operator()(const GraphicsBufferWrapper &) = delete;
    GraphicsBufferWrapper &operator()(GraphicsBufferWrapper &&) = delete;

private:
    GraphicsBufferWrapper() = default;

    std::shared_ptr<GraphicsBuffer> m_Buffer;
    std::shared_ptr<GraphicsBackendBufferInfo> m_BufferInfo;
    std::vector<uint8_t> m_Data;
    std::string m_Name;
};

#endif //RENDER_ENGINE_GRAPHICS_BUFFER_WRAPPER_H
