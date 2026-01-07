#include "ui_manager.h"
#include "ui_image.h"
#include "ui_text.h"
#include "ui_button.h"
#include "ui_text_field.h"
#include "ui_event_info.h"
#include "font/font.h"
#include "resources/resources.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "editor/profiler/profiler.h"
#include "texture_2d/texture_2d.h"

std::shared_ptr<Font> UIManager::s_Font;
std::shared_ptr<UIElement> UIManager::s_Root;
std::vector<UIElement*> UIManager::s_Elements;
std::shared_ptr<UIElement> UIManager::s_FocusedElement;
std::shared_ptr<UIElement> UIManager::s_HoveredElement;

Vector2 UIManager::s_ReferenceSize(0, 0);

void UIManager::Initialize(const Vector2& referenceSize)
{
    s_ReferenceSize = referenceSize;
    s_Root = std::make_shared<UIElement>(Vector2(0, 0), s_ReferenceSize);
}

void UIManager::Update()
{
    Profiler::Marker _("UIManager::Update");

    {
        Profiler::Marker collectMarker("Collect Elements");

        s_Elements.clear();
        CollectElements(*s_Root);
    }

    {
        Profiler::Marker inputMarker("Handle Input");

        auto ToUIPosition = [](const Vector2& inputPosition)
        {
            Vector2 uiPosition = inputPosition;
            uiPosition.x = uiPosition.x / Graphics::GetScreenWidth() * s_ReferenceSize.x;
            uiPosition.y = (1 - uiPosition.y / Graphics::GetScreenHeight()) * s_ReferenceSize.y;
            return uiPosition;
        };

        auto HandlePointerEvent = [](const Vector2& position, UIEventType eventType)
        {
            UIEventInfo eventInfo{};
            eventInfo.Position = position;
            eventInfo.Type = eventType;

            std::shared_ptr<UIElement> element = HandleEvent(eventInfo, s_Root);
            ChangeFocus(element);
        };

        auto HandleHoverEvent = [](const Vector2& position)
        {
            UIEventInfo eventInfo{};
            eventInfo.Position = position;
            eventInfo.Type = UIEventType::HOVER;

            std::shared_ptr<UIElement> element = HandleEvent(eventInfo, s_Root);
            if (element != s_HoveredElement)
            {
                if (s_HoveredElement)
                    s_HoveredElement->LoseHover();
                s_HoveredElement = element;
            }
        };

        const Vector2 mousePos = ToUIPosition(Input::GetMousePosition());
        if (Input::GetMouseButtonDown(Input::MouseButton::LEFT))
            HandlePointerEvent(mousePos, UIEventType::POINTER_DOWN);
        else if (Input::GetMouseButtonUp(Input::MouseButton::LEFT))
            HandlePointerEvent(mousePos, UIEventType::POINTER_UP);
        HandleHoverEvent(mousePos);

        const std::vector<Input::Touch>& touches = Input::GetTouches();
        for (const Input::Touch& touch : touches)
        {
            const Vector2 touchPos = ToUIPosition(touch.Position);
            if (touch.State == Input::TouchState::DOWN)
                HandlePointerEvent(touchPos, UIEventType::POINTER_DOWN);
            else if (touch.State == Input::TouchState::UP)
                HandlePointerEvent(touchPos, UIEventType::POINTER_UP);
            else
                HandleHoverEvent(touchPos);
        }

        if (s_FocusedElement)
        {
            const std::unordered_map<Input::SpecialKey, Input::KeyboardKeyState> specialKeys = Input::GetSpecialKeys();
            for (const auto& pair : specialKeys)
            {
                const Input::KeyboardKeyState& keyState = pair.second;
                if (keyState.State == 0)
                    continue;

                UIEventInfo eventInfo;
                eventInfo.KeyState = keyState;
                eventInfo.Type = UIEventType::SPECIAL_KEY;
                HandleEvent(eventInfo, s_FocusedElement);
                if (eventInfo.LoseFocus)
                    ChangeFocus(nullptr);
            }

            std::unordered_set<unsigned char> charInputs = Input::GetCharInputs();
            for (unsigned char ch : charInputs)
            {
                UIEventInfo eventInfo;
                eventInfo.KeyState.Char = ch;
                eventInfo.Type = UIEventType::CHAR_INPUT;
                HandleEvent(eventInfo, s_FocusedElement);
                if (eventInfo.LoseFocus)
                    ChangeFocus(nullptr);
            }
        }
    }
}

void UIManager::DestroyUI()
{
    s_FocusedElement = nullptr;
    s_Elements.clear();

    std::vector<std::shared_ptr<UIElement>> children = std::move(s_Root->m_Children);
    s_Root->m_Children.clear();

    for (std::shared_ptr<UIElement>& child : children)
        child->Destroy();
}

std::shared_ptr<UIImage> UIManager::CreateImage(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UIImage> uiImage = std::make_shared<UIImage>(position, size, image);
    uiImage->SetParent(parent != nullptr ? parent : s_Root);
    return uiImage;
}

std::shared_ptr<UIText> UIManager::CreateText(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::string& text, uint16_t fontSize)
{
    if (!s_Font)
        s_Font = Resources::Load<Font>("core_resources/fonts/Inter.ttf");

    std::shared_ptr<UIText> uiText = std::make_shared<UIText>(position, size, text, s_Font);
    uiText->SetParent(parent != nullptr ? parent : s_Root);
    uiText->SetFontSize(fontSize);
    return uiText;
}

std::shared_ptr<UIButton> UIManager::CreateButton(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::string& text, uint16_t fontSize, const std::shared_ptr<Texture2D> image)
{
     std::shared_ptr<UIButton> uiButton = std::make_shared<UIButton>(position, size);
     uiButton->SetParent(parent != nullptr ? parent : s_Root);

     std::shared_ptr<UIImage> uiImage = UIManager::CreateImage(uiButton, Vector2(0, 0), size, image);
     std::shared_ptr<UIText> uiText = UIManager::CreateText(uiButton, Vector2(0, 0), size, text, fontSize);
     uiText->SetHorizontalAlignment(UIText::HorizontalAlignment::MIDDLE);
     uiText->SetVerticalAlignment(UIText::VerticalAlignment::MIDDLE);

     uiButton->m_Image = uiImage;
     uiButton->m_Text = uiText;

     return uiButton;
}

std::shared_ptr<UITextField> UIManager::CreateTextField(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, uint16_t fontSize, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UITextField> uiTextField = std::make_shared<UITextField>(position, size);
    uiTextField->SetParent(parent != nullptr ? parent : s_Root);

    std::shared_ptr<UIImage> backgroundImage = UIManager::CreateImage(uiTextField, Vector2(0, 0), size, image);
    std::shared_ptr<UIText> uiText = UIManager::CreateText(uiTextField, Vector2(0, 0), size, "", fontSize);
    std::shared_ptr<UIImage> cursorImage = UIManager::CreateImage(uiTextField, Vector2(0, 0), Vector2(1, size.y), Texture2D::White());

    const Vector4 defaultColor = Vector4(0.2f, 0.2f, 0.2f, 1);
    uiText->Color = defaultColor;
    cursorImage->Color = defaultColor;
    cursorImage->Active = false;

    uiTextField->m_BackgroundImage = backgroundImage;
    uiTextField->m_CursorImage = cursorImage;
    uiTextField->m_Text = uiText;

    return uiTextField;
}

void UIManager::CollectElements(UIElement& element)
{
    if (!element.Active)
        return;

    s_Elements.push_back(&element);
    for (std::shared_ptr<UIElement>& child : element.m_Children)
    {
        if (child)
        {
            child->m_GlobalPosition = element.m_GlobalPosition + child->Position;
            CollectElements(*child);
        }
    }
}

std::shared_ptr<UIElement> UIManager::HandleEvent(UIEventInfo& eventInfo, std::shared_ptr<UIElement>& element)
{
    if ((eventInfo.Type == UIEventType::POINTER_DOWN ||
         eventInfo.Type == UIEventType::POINTER_UP ||
         eventInfo.Type == UIEventType::HOVER) &&
        (eventInfo.Position.x < element->m_GlobalPosition.x ||
        eventInfo.Position.y < element->m_GlobalPosition.y ||
        eventInfo.Position.x > element->m_GlobalPosition.x + element->Size.x ||
        eventInfo.Position.y > element->m_GlobalPosition.y + element->Size.y))
        return nullptr;

    element->HandleEvent(eventInfo);
    if (eventInfo.Consumed)
        return element;

    for (int i = element->m_Children.size() - 1; i >= 0; --i)
    {
        std::shared_ptr<UIElement> consumer = HandleEvent(eventInfo, element->m_Children[i]);
        if (consumer)
            return consumer;
    }

    return nullptr;
}

void UIManager::ChangeFocus(const std::shared_ptr<UIElement>& newFocusedElement)
{
    if (newFocusedElement == s_FocusedElement)
        return;

    if (s_FocusedElement)
        s_FocusedElement->LoseFocus();
    s_FocusedElement = newFocusedElement;
}