#include "texture_view.h"

TextureView::TextureView(const std::shared_ptr<Texture>& texture, const GraphicsBackendTextureViewDescriptor& descriptor, const std::string& name) :
	m_TextureViewDescriptor(descriptor),
	m_Texture(texture)
{
	m_TextureView = GraphicsBackend::Current()->CreateTextureView(descriptor, texture->GetBackendTexture(), name);
}

TextureView::~TextureView()
{
	GraphicsBackend::Current()->DeleteTextureView(m_TextureView);
}
