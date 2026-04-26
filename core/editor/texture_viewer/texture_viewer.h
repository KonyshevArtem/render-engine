#ifndef RENDER_ENGINE_TEXTURE_VIEWER_H
#define RENDER_ENGINE_TEXTURE_VIEWER_H

#include "texture/texture.h"
#include "shader/shader.h"
#include "graphics_buffer/graphics_buffer.h"
#include "file_system/file_watcher.h"

#include <memory>
#include <functional>

class TextureViewer
{
public:
	static void RegisterTexture(const std::shared_ptr<Texture>& texture, const std::string& name);

	static void SetSelectedTextureName(const std::string& name);
	static void SetColorMask(Vector4I mask);
	static void SetTextureRegisteredCallback(std::function<void(const std::string&)> callback);

	static std::shared_ptr<Texture> GetSelectedTextureCopy();

private:
	static std::string s_SelectedTextureName;
	static Vector4I s_ColorMask;
	static std::function<void(const std::string&)> s_TextureRegisteredCallback;

	static std::shared_ptr<Texture> s_SelectedTextureCopy;
	static std::shared_ptr<Shader> s_CopyShader;
	static std::shared_ptr<GraphicsBuffer> s_DataBuffer;
	static FileWatcher s_FileWatcher;
};

#endif