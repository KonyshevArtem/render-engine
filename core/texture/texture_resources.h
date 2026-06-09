#ifndef GRAPHICS_BACKEND_TEXTURE_RESOURCES_H
#define GRAPHICS_BACKEND_TEXTURE_RESOURCES_H

#include "texture.h"
#include "texture_view.h"

struct TextureResources
{
	std::shared_ptr<Texture> Texture;
	std::shared_ptr<TextureView> View;
	std::shared_ptr<TextureView> RWView;

	void Clear()
	{
		Texture = nullptr;
		View = nullptr;
		RWView = nullptr;
	}
};

#endif // GRAPHICS_BACKEND_TEXTURE_RESOURCES_H