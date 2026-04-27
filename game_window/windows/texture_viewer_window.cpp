#ifdef ENABLE_IMGUI

#include "texture_viewer_window.h"
#include "imgui.h"
#include "graphics_backend_api.h"
#include "texture/texture.h"
#include "editor/texture_viewer/texture_viewer.h"

TextureViewerWindow::TextureViewerWindow() :
	BaseWindow(600, 400, "Texture Viewer", typeid(TextureViewerWindow).hash_code()),
	m_SelectedTextureName("Search..."),
	m_ColorMask(1, 1, 1, 1),
	m_MinMaxValues(0, 1),
	m_LinearizeDepth(false)
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

void TextureViewerWindow::OnTextureRegistered(const std::string& textureName)
{
	m_RegisteredTextures.insert(textureName);
}

void TextureViewerWindow::DrawInternal()
{
	const std::shared_ptr<Texture> selectedTexture = TextureViewer::GetSelectedTextureCopy();
    if (!selectedTexture)
        return;

	const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
	const float maxW = contentRegion.x;
	const float maxH = contentRegion.y;
	const float aspect = static_cast<float>(selectedTexture->GetWidth()) / static_cast<float>(selectedTexture->GetHeight());
	float displayW = maxW;
	float displayH = displayW / aspect;
	if (displayH > maxH)
	{
		displayH = maxH;
		displayW = displayH * aspect;
	}

	ImGui::Image(GraphicsBackend::Current()->GetImGuiTextureId(selectedTexture->GetBackendTexture()), ImVec2(displayW, displayH));
}

#endif