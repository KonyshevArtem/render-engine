#ifndef RENDER_ENGINE_TEXTURE_VIEWER_WINDOW_H
#define RENDER_ENGINE_TEXTURE_VIEWER_WINDOW_H

#ifdef ENABLE_IMGUI

#include "base_window.h"
#include "vector4/vector4.h"
#include "vector2/vector2.h"

#include <string>
#include <unordered_set>

class Texture;

class TextureViewerWindow : public BaseWindow
{
public:
    TextureViewerWindow();
    ~TextureViewerWindow() override;

protected:
    void DrawInternal() override;
    void DrawTopBar() override;

private:
    std::string m_SelectedTextureName;
	std::unordered_set<std::string> m_RegisteredTextures;
    Vector4I m_ColorMask;
    Vector2 m_MinMaxValues;
    bool m_LinearizeDepth;

    void DrawTextureSelector();
    void DrawColorMaskSelector();
    void DrawMinMaxValuesSelector();
    void OnTextureRegistered(const std::string& textureName);
};

#endif // ENABLE_IMGUI

#endif // RENDER_ENGINE_TEXTURE_VIEWER_WINDOW_H