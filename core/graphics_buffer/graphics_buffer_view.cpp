#include "graphics_buffer_view.h"

GraphicsBufferView::GraphicsBufferView(const std::shared_ptr<GraphicsBuffer>& buffer, const GraphicsBackendBufferViewDescriptor& descriptor, const std::string& name) :
	m_BufferViewDescriptor(descriptor),
	m_Buffer(buffer)
{
	m_BufferView = GraphicsBackend::Current()->CreateBufferView(descriptor, buffer->GetBackendBuffer(), name);
}

GraphicsBufferView::~GraphicsBufferView()
{
	GraphicsBackend::Current()->DeleteBufferView(m_BufferView);
}
