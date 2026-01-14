#include "ui_native_keyboard.h"
#include "ui_text_field.h"
#include "ui_manager.h"
#include "native_bridge/native_bridge_android.h"
#include "native_bridge/native_bridge_apple.h"

namespace UINativeKeyboard_Local
{
    std::shared_ptr<UITextField> s_CurrentTextField;

    void OnNativeKeyboardTextChanged(const char* text)
    {
        if (s_CurrentTextField)
            s_CurrentTextField->SetText(text);
    }

    void OnFinishEdit(bool done)
    {
        if (done && s_CurrentTextField)
            s_CurrentTextField->Done();
        s_CurrentTextField = nullptr;

        UIManager::ResetFocus();
    }
}

void UINativeKeyboard::ShowKeyboard(std::shared_ptr<UITextField> textField)
{
    UINativeKeyboard_Local::s_CurrentTextField = textField;

#if RENDER_ENGINE_ANDROID
    NativeBridgeAndroid::ShowNativeKeyboard(
            textField->GetText().c_str(),
            textField.get(),
            reinterpret_cast<void*>(&UINativeKeyboard_Local::OnNativeKeyboardTextChanged),
            reinterpret_cast<void*>(&UINativeKeyboard_Local::OnFinishEdit)
        );
#elif RENDER_ENGINE_APPLE
    NativeBridgeApple::ShowNativeKeyboard(
            textField->GetText().c_str(),
            UINativeKeyboard_Local::OnNativeKeyboardTextChanged,
            UINativeKeyboard_Local::OnFinishEdit
    );
#endif
}
