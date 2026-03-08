#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_DESCRIPTOR_H

#include "enums/comparison_function.h"
#include "enums/texture_filtering_mode.h"
#include "enums/texture_wrap_mode.h"

struct GraphicsBackendSamplerDescriptor
{
	TextureWrapMode WrapMode = TextureWrapMode::CLAMP_TO_EDGE;
	TextureFilteringMode FilteringMode = TextureFilteringMode::LINEAR_MIPMAP_NEAREST;
	ComparisonFunction ComparisonFunction = ComparisonFunction::NONE;
	float BorderColor[4] = {0, 0, 0, 0};
	bool HasBorderColor = false;
	int MinLod = 0;
};

#endif