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

    inline const GraphicsBackendBufferView& GetBackendBufferView() const
    {
        return m_BufferView;
    }

private:
    GraphicsBackendBufferView m_BufferView;
};

#endif //RENDER_ENGINE_GRAPHICS_BUFFER_VIEW_H
