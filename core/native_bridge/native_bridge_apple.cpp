#ifdef RENDER_ENGINE_APPLE

#include "native_bridge_apple.h"

namespace NativeBridgeApple_Local
{
    ShowNativeKeyboardCallback s_ShowNativeKeyboardCallback;

    NativeBridgeApple::TextChangedCallback s_TextChangedCallback;
    NativeBridgeApple::FinishEditCallback s_FinishEditCallback;
}

void SetShowNativeKeyboardCallback(ShowNativeKeyboardCallback callback)
{
    NativeBridgeApple_Local::s_ShowNativeKeyboardCallback = callback;
}

void NativeKeyboardTextChanged(const char* text)
{
    NativeBridgeApple_Local::s_TextChangedCallback(text);
}

void NativeKeyboardFinishEdit(bool done)
{
    NativeBridgeApple_Local::s_FinishEditCallback(done);
}

void NativeBridgeApple::ShowNativeKeyboard(const char *text, TextChangedCallback textChangedCallback, FinishEditCallback finishEditCallback)
{
    NativeBridgeApple_Local::s_TextChangedCallback = textChangedCallback;
    NativeBridgeApple_Local::s_FinishEditCallback = finishEditCallback;

    NativeBridgeApple_Local::s_ShowNativeKeyboardCallback(text);
}

#endif