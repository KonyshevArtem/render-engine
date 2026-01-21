#ifndef RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H
#define RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H

#if RENDER_ENGINE_ANDROID

#include "native_bridge_common.h"
#include <cstdio>

class NativeBridgeAndroid
{
public:
    static void ShowNativeKeyboard(const wchar_t* text, TextChangedCallback textChangedCallback, FinishEditCallback finishEditCallback);

    static void NativeKeyboardTextChanged(const char16_t* text, size_t length);
    static void NativeKeyboardFinishEdit(bool done);
};

#endif

#endif //RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H
