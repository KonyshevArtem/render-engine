#ifndef RENDER_ENGINE_GRAPHICS_BUFFER_VIEW_H
#define RENDER_ENGINE_GRAPHICS_BUFFER_VIEW_H

#include "graphics_backend_api.h"
#include "graphics_buffer.h"
#include "types/graphics_backend_buffer_view.h"
#include "types/graphics_backend_buffer_view_descriptor.h"

#include <string>

class GraphicsBufferView
{
public:
    GraphicsBufferView(const std::shared_ptr<GraphicsBuffer>& buffer, const GraphicsBackendBufferViewDescriptor& descriptor, const std::string& name);
    ~GraphicsBufferView();

    const GraphicsBackendBufferView& GetBackendBufferView() const
    {
        return m_BufferView;
    }

    const GraphicsBackendBufferViewDescriptor& GetDescriptor() const
    {
        return m_BufferViewDescriptor;
    }

    std::shared_ptr<GraphicsBuffer> GetBuffer() const
    {
        return m_Buffer.expired() ? nullptr : m_Buffer.lock();
    }

private:
    GraphicsBackendBufferView m_BufferView;
    GraphicsBackendBufferViewDescriptor m_BufferViewDescriptor;

    std::weak_ptr<GraphicsBuffer> m_Buffer;
};

#endif //RENDER_ENGINE_GRAPHICS_BUFFER_VIEW_H
