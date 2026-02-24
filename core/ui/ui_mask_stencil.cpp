#include "ui_mask_stencil.h"

std::shared_ptr<UIMaskStencil> UIMaskStencil::Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, bool open)
{
	std::shared_ptr<UIMaskStencil> maskStencil = std::shared_ptr<UIMaskStencil>(new UIMaskStencil(position, size, open));
	maskStencil->SetParent(parent);

	return maskStencil;
}

UIMaskStencil::UIMaskStencil(const Vector2& position, const Vector2& size, bool open)
	: UIElement(position, size),
	Open(open)
{
}

