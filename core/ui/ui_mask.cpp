#include "ui_mask.h"
#include "ui_mask_stencil.h"

std::shared_ptr<UIMask> UIMask::Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size)
{
    std::shared_ptr<UIMask> uiMask = std::shared_ptr<UIMask>(new UIMask(position, size));
    uiMask->SetParent(parent);

    std::shared_ptr<UIMaskStencil> openStencil = UIMaskStencil::Create(uiMask, Vector2(0, 0), size, true);
    std::shared_ptr<UIMaskStencil> closeStencil = UIMaskStencil::Create(uiMask, Vector2(0, 0), size, false);

    uiMask->m_MaskStencils[0] = openStencil;
    uiMask->m_MaskStencils[1] = closeStencil;

    return uiMask;
}

void UIMask::Update()
{
	UIElement::Update();

    for (const std::shared_ptr<UIMaskStencil>& maskStencil : m_MaskStencils)
        maskStencil->Size = Size;
}

void UIMask::AddChild(const std::shared_ptr<UIElement>& child)
{
    if (m_Children.size() >= 2)
        m_Children.insert(m_Children.end() - 1, child);
    else
        m_Children.push_back(child);
}

UIMask::UIMask(const Vector2& position, const Vector2& size)
	: UIElement(position, size)
{
}
