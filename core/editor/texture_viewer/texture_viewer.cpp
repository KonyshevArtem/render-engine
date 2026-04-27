#include "texture_viewer.h"

#include "texture_2d/texture_2d.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "vector2/vector2.h"
#include "vector3/vector3.h"

std::string TextureViewer::s_SelectedTextureName;
Vector4I TextureViewer::s_ColorMask;
Vector2 TextureViewer::s_MinMaxValues;
bool TextureViewer::s_LinearizeDepth;
std::function<void(const std::string&)> TextureViewer::s_TextureRegisteredCallback = nullptr;

std::shared_ptr<Texture> TextureViewer::s_SelectedTextureCopy = nullptr;
std::shared_ptr<Shader> TextureViewer::s_CopyShader = nullptr;
std::shared_ptr<GraphicsBuffer> TextureViewer::s_DataBuffer = nullptr;
FileWatcher TextureViewer::s_FileWatcher;

void TextureViewer::RegisterTexture(const std::shared_ptr<Texture>& texture, const std::string& name)
{
	if (s_TextureRegisteredCallback)
		s_TextureRegisteredCallback(name);

	if (s_SelectedTextureName == name)
	{
		if (!s_CopyShader || s_FileWatcher.FilesChanged())
		{
			if (!s_CopyShader)
				s_FileWatcher.AddFile("core_resources/shaders/editor/texture_viewer/texture_viewer_copy.hlsl");

			s_CopyShader = Shader::Load("core_resources/shaders/editor/texture_viewer/texture_viewer_copy", {});
		}

		if (!s_CopyShader)
			return;

		struct
		{
			Vector2I Size;
			Vector2 MinMax;

			Vector4I ColorMask;

			Vector3 Padding0;
			uint32_t ShouldLinearizeDepth;
		} data{};

		if (!s_DataBuffer)
		{
			GraphicsBackendBufferDescriptor bufferDesc{};
			bufferDesc.AllowCPUWrites = true;
			bufferDesc.Size = sizeof(data);

			s_DataBuffer = std::make_shared<GraphicsBuffer>(bufferDesc, "TextureViewer/Data");
		}

		GraphicsBackendTextureDescriptor desc = texture->GetTextureDescriptor();
		desc.ReadWrite = true;
		desc.RenderTarget = false;
		if (GraphicsBackend::Current()->IsDepthFormat(desc.Format))
			desc.Format = TextureInternalFormat::R32F;

		if (!s_SelectedTextureCopy || s_SelectedTextureCopy->GetTextureDescriptor() != desc)
			s_SelectedTextureCopy = Texture2D::Create(desc, "Texture Viewer Copy");

		GraphicsBackend::Current()->BeginComputePass("Texture Viewer Copy");

		data.Size = Vector2I(texture->GetWidth(), texture->GetHeight());
		data.ColorMask = s_ColorMask;
		data.MinMax = s_MinMaxValues;
		data.ShouldLinearizeDepth = s_LinearizeDepth;

		GraphicsBackend::Current()->SetBufferData(s_DataBuffer->GetBackendBuffer(), 0, sizeof(data), &data);
		GraphicsBackend::Current()->BindConstantBuffer(s_DataBuffer->GetBackendBuffer(), 0, 0, sizeof(data));

		GraphicsBackend::Current()->BindTexture(texture->GetBackendTexture(), 0);
		GraphicsBackend::Current()->BindRWTexture(s_SelectedTextureCopy->GetBackendTexture(), 0);

		GraphicsBackend::Current()->UseProgram(s_CopyShader->GetProgram());
		GraphicsBackend::Current()->Dispatch(data.Size.x, data.Size.y, 1);

		GraphicsBackend::Current()->EndComputePass();
	}
}

void TextureViewer::SetSelectedTextureName(const std::string& name)
{
	s_SelectedTextureName = name;
}

void TextureViewer::SetColorMask(Vector4I mask)
{
	s_ColorMask = mask;
}

void TextureViewer::SetMinMaxValues(Vector2 minMax)
{
	s_MinMaxValues = minMax;
}

void TextureViewer::SetLinearizeDepth(bool linearize)
{
	s_LinearizeDepth = linearize;
}

void TextureViewer::SetTextureRegisteredCallback(std::function<void(const std::string&)> callback)
{
	s_TextureRegisteredCallback = std::move(callback);
}

std::shared_ptr<Texture> TextureViewer::GetSelectedTextureCopy()
{
	return s_SelectedTextureCopy;
}
