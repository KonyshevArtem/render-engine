#ifndef RENDER_ENGINE_TEXTURE_VIEWER_H
#define RENDER_ENGINE_TEXTURE_VIEWER_H

#include "texture/texture_resources.h"
#include "shader/shader.h"
#include "graphics_buffer/graphics_buffer.h"
#include "file_watcher.h"
#include "vector2/vector2.h"
#include "vector4/vector4.h"

#include <memory>
#include <functional>

class TextureViewer
{
public:
	static void RegisterTexture(const std::shared_ptr<TextureView>& textureView, const std::string& name);

	static void SetSelectedTextureName(const std::string& name);
	static void SetColorMask(Vector4I mask);
	static void SetMinMaxValues(Vector2 minMax);
	static void SetLinearizeDepth(bool linearize);
	static void SetTextureSlice(int slice);
	static void SetTextureRegisteredCallback(std::function<void(const std::string&)> callback);

	static std::shared_ptr<TextureView> GetSelectedTextureCopy();

private:
	static std::string s_SelectedTextureName;
	static Vector4I s_ColorMask;
	static Vector2 s_MinMaxValues;
	static bool s_LinearizeDepth;
	static int s_TextureSlice;
	static std::function<void(const std::string&)> s_TextureRegisteredCallback;

	static TextureResources s_SelectedTextureCopy;
	static std::shared_ptr<Shader> s_CopyShaders[2];
	static std::shared_ptr<GraphicsBuffer> s_DataBuffer;
	static FileWatcher s_FileWatcher;
};

#endif