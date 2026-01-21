#include "ui_manager.h"
#include "ui_image.h"
#include "ui_text_field.h"
#include "ui_event_info.h"
#include "resources/resources.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "editor/profiler/profiler.h"

std::shared_ptr<UIElement> UIManager::s_Root;
std::vector<UIElement*> UIManager::s_Elements;
std::shared_ptr<UIElement> UIManager::s_FocusedElement;
std::shared_ptr<UIElement> UIManager::s_HoveredElement;

Vector2 UIManager::s_ReferenceSize(0, 0);

void UIManager::Initialize(float referenceHeight)
{
    s_ReferenceSize = Vector2(0, referenceHeight);
    s_Root = std::make_shared<UIElement>(Vector2(0, 0), s_ReferenceSize);
}

void UIManager::Update()
{
    Profiler::Marker _("UIManager::Update");

    float aspect = static_cast<float>(Graphics::GetScreenWidth()) / static_cast<float>(Graphics::GetScreenHeight());
    s_ReferenceSize.x = aspect * s_ReferenceSize.y;
    s_Root->Size = s_ReferenceSize;

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

            std::unordered_set<wchar_t> charInputs = Input::GetCharInputs();
            for (wchar_t ch : charInputs)
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

void UIManager::ResetFocus()
{
    ChangeFocus(nullptr);
}

std::shared_ptr<UIElement> UIManager::GetRoot()
{
    return s_Root;
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