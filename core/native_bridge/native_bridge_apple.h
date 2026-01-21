#ifndef RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
#define RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H

#if RENDER_ENGINE_APPLE

#include "native_bridge_common.h"

typedef void (*ShowNativeKeyboardCallback)(const wchar_t* text);

void SetShowNativeKeyboardCallback(ShowNativeKeyboardCallback fn);
void NativeKeyboardTextChanged(const wchar_t* text);
void NativeKeyboardFinishEdit(bool done);

class NativeBridgeApple
{
public:
    static void ShowNativeKeyboard(const wchar_t* text, TextChangedCallback textChangedCallback, FinishEditCallback finishEditCallback);
};

#endif

#endif //RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
