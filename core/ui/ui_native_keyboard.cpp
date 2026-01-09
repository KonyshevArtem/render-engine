#include "ui_native_keyboard.h"
#include "ui_text_field.h"
#include "ui_manager.h"
#include "native_bridge/native_bridge_android.h"

namespace UINativeKeyboard_Local
{
    void OnNativeKeyboardTextChanged(void* textFieldPtr, const char* text)
    {
        UITextField* textField = static_cast<UITextField*>(textFieldPtr);
        textField->SetText(text);
    }

    void OnFinishEdit(void* textFieldPtr, bool done)
    {
        if (done)
        {
            UITextField* textField = static_cast<UITextField*>(textFieldPtr);
            textField->Done();
        }

        UIManager::ResetFocus();
    }
}

void UINativeKeyboard::ShowKeyboard(std::shared_ptr<UITextField> textField)
{
#if RENDER_ENGINE_ANDROID
    NativeBridgeAndroid::ShowNativeKeyboard(
            textField->GetText().c_str(),
            textField.get(),
            reinterpret_cast<void*>(&UINativeKeyboard_Local::OnNativeKeyboardTextChanged),
            reinterpret_cast<void*>(&UINativeKeyboard_Local::OnFinishEdit)
        );
#endif
}
