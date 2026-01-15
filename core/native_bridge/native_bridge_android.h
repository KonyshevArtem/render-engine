#ifndef RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H
#define RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H

#ifdef RENDER_ENGINE_ANDROID

#include "native_bridge_common.h"

class NativeBridgeAndroid
{
public:
    static void ShowNativeKeyboard(const char* text, TextChangedCallback textChangedCallback, FinishEditCallback finishEditCallback);

    static void NativeKeyboardTextChanged(const char *text);
    static void NativeKeyboardFinishEdit(bool done);
};

#endif

#endif //RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H
