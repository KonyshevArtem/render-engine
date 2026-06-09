#include "imgui_internal.h"
#ifdef ENABLE_IMGUI

#include "texture_viewer_window.h"
#include "imgui.h"
#include "graphics_backend_api.h"
#include "texture/texture.h"
#include "editor/texture_viewer/texture_viewer.h"

TextureViewerWindow::TextureViewerWindow() :
	BaseWindow(650, 400, "Texture Viewer", typeid(TextureViewerWindow).hash_code()),
	m_SelectedTextureName("Search..."),
	m_ColorMask(1, 1, 1, 1),
	m_MinMaxValues(0, 1),
	m_LinearizeDepth(false),
	m_TextureSlice(0),
	m_Zoom(1),
	m_ZoomCenter(0.5f, 0.5f)
{
	TextureViewer::SetTextureRegisteredCallback([this](const std::string& textureName) {OnTextureRegistered(textureName); });
	TextureViewer::SetColorMask(m_ColorMask);
}

TextureViewerWindow::~TextureViewerWindow()
{
	TextureViewer::SetTextureRegisteredCallback(nullptr);
	TextureViewer::SetSelectedTextureName("");
}

void TextureViewerWindow::DrawTopBar()
{
	DrawTextureSelector();
	DrawColorMaskSelector();
	DrawMinMaxValuesSelector();
	DrawTextureSliceSelector();
}

void TextureViewerWindow::DrawTextureSelector()
{
	ImGui::PushItemWidth(300);
	if (!ImGui::BeginCombo("Texture select", m_SelectedTextureName.c_str()))
		return;

	static char searchBuf[128] = {};
	if (ImGui::IsWindowAppearing())
	{
		ImGui::SetKeyboardFocusHere();
		memset(searchBuf, 0, sizeof(searchBuf));
	}

	ImGui::InputText("##search", searchBuf, sizeof(searchBuf));
	ImGui::Separator();

	const std::string query(searchBuf);
	for (const std::string& name : m_RegisteredTextures)
	{
		const bool matches = query.empty() || name.find(query) != std::string::npos;
		if (!matches) 
			continue;
		
		if (ImGui::Selectable(name.c_str(), false)) 
		{
			m_SelectedTextureName = name;
			TextureViewer::SetSelectedTextureName(m_SelectedTextureName);

			m_Zoom = 1;
			m_ZoomCenter = Vector2(0.5f, 0.5f);

			ImGui::CloseCurrentPopup();
		}
	}

	ImGui::EndCombo();
	ImGui::PopItemWidth();
}

void TextureViewerWindow::DrawColorMaskSelector()
{
	const char* channels[] = { "R", "G", "B", "A" };
	constexpr ImVec4 activeColors[] = {
		ImVec4(0.8f, 0.1f, 0.1f, 1.0f),
		ImVec4(0.1f, 0.7f, 0.1f, 1.0f),
		ImVec4(0.1f, 0.3f, 0.9f, 1.0f),
		ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
	};
	constexpr ImVec4 inactiveColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

	ImGui::Separator();
	ImGui::PushID("Color Mask");
	for (int i = 0; i < 4; i++) 
	{
		if (i > 0) 
			ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, m_ColorMask[i] ? activeColors[i] : inactiveColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_ColorMask[i] ? activeColors[i] : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));

		if (ImGui::Button(channels[i], ImVec2(24, 24)))
		{
			m_ColorMask[i] ^= 1;
			TextureViewer::SetColorMask(m_ColorMask);
		}

		ImGui::PopStyleColor(2);
	}
	ImGui::PopID();
	
	ImGui::SameLine();
	ImGui::TextUnformatted("Color Mask");
}

void TextureViewerWindow::DrawMinMaxValuesSelector()
{
	ImGui::Separator();
	ImGui::PushItemWidth(60);

	ImGui::SameLine();
	ImGui::InputFloat("Min", &m_MinMaxValues.x);

	ImGui::SameLine();
	ImGui::InputFloat("Max", &m_MinMaxValues.y);

	ImGui::SameLine();
	if (ImGui::RadioButton("Linear Depth", m_LinearizeDepth))
		m_LinearizeDepth = !m_LinearizeDepth;

	ImGui::PopItemWidth();

	TextureViewer::SetMinMaxValues(m_MinMaxValues);
	TextureViewer::SetLinearizeDepth(m_LinearizeDepth);
}

void TextureViewerWindow::DrawTextureSliceSelector()
{
	ImGui::Separator();
	ImGui::PushItemWidth(100);

	ImGui::SameLine();
	ImGui::InputInt("Slice", &m_TextureSlice);

	if (m_TextureSlice < 0)
		m_TextureSlice = 0;

	ImGui::PopItemWidth();

	TextureViewer::SetTextureSlice(m_TextureSlice);
}

void TextureViewerWindow::OnTextureRegistered(const std::string& textureName)
{
	m_RegisteredTextures.insert(textureName);
}

void TextureViewerWindow::DrawInternal()
{
	const std::shared_ptr<TextureView> selectedTextureView = TextureViewer::GetSelectedTextureCopy();
    if (!selectedTextureView || !selectedTextureView->GetTexture())
        return;

	const std::shared_ptr<Texture> texture = selectedTextureView->GetTexture();

	const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
	const float maxW = contentRegion.x;
	const float maxH = contentRegion.y - ImGui::GetTextLineHeight();
	const float aspect = static_cast<float>(texture->GetWidth()) / static_cast<float>(texture->GetHeight());
	ImVec2 imageSize{ maxW, maxW / aspect };
	if (imageSize.y > maxH)
	{
		imageSize.y = maxH;
		imageSize.x = maxH * aspect;
	}

	ImGui::BeginChild("##image", imageSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	float uvSize = 1.0f / m_Zoom;
	const ImVec2 uv0(m_ZoomCenter.x - uvSize * 0.5f, m_ZoomCenter.y - uvSize * 0.5f);
	const ImVec2 uv1(m_ZoomCenter.x + uvSize * 0.5f, m_ZoomCenter.y + uvSize * 0.5f);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest);
	ImGui::Image(GraphicsBackend::Current()->GetImGuiTextureId(selectedTextureView->GetBackendTextureView()), imageSize, uv0, uv1);
	drawList->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear);

	if (ImGui::IsItemHovered())
	{
		const float wheel = ImGui::GetIO().MouseWheel;

		const ImVec2 mousePos = ImGui::GetIO().MousePos;
		const ImVec2 itemMin = ImGui::GetItemRectMin();
		const ImVec2 itemSize = ImGui::GetItemRectSize();

		if (wheel != 0.0f)
		{
			const ImVec2 mousePosUV(
				uv0.x + (mousePos.x - itemMin.x) / itemSize.x * (uv1.x - uv0.x),
				uv0.y + (mousePos.y - itemMin.y) / itemSize.y * (uv1.y - uv0.y)
			);

			m_Zoom = ImClamp(m_Zoom * (1.0f + wheel * 0.1f), 1.0f, 32.0f);
			uvSize = 1.0f / m_Zoom;

			const ImVec2 distance(
				mousePosUV.x - m_ZoomCenter.x,
				mousePosUV.y - m_ZoomCenter.y
			);

			m_ZoomCenter.x = ImClamp(m_ZoomCenter.x + distance.x * 0.5f * (wheel > 0 ? 1 : -1), uvSize * 0.5f, 1.0f - uvSize * 0.5f);
			m_ZoomCenter.y = ImClamp(m_ZoomCenter.y + distance.y * 0.5f * (wheel > 0 ? 1 : -1), uvSize * 0.5f, 1.0f - uvSize * 0.5f);
		}

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			const ImVec2 delta = ImGui::GetIO().MouseDelta;
			m_ZoomCenter.x = ImClamp(m_ZoomCenter.x - delta.x / itemSize.x * uvSize, uvSize * 0.5f, 1.0f - uvSize * 0.5f);
			m_ZoomCenter.y = ImClamp(m_ZoomCenter.y - delta.y / itemSize.y * uvSize, uvSize * 0.5f, 1.0f - uvSize * 0.5f);
		}
	}

	ImGui::EndChild();

	ImGui::Text("Size: %ix%i", texture->GetWidth(), texture->GetHeight());
}

#endif