#ifndef RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
#define RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H

#ifdef RENDER_ENGINE_APPLE

typedef void (*ShowNativeKeyboardCallback)(const char* text);

void SetShowNativeKeyboardCallback(ShowNativeKeyboardCallback fn);
void NativeKeyboardTextChanged(const char *text);
void NativeKeyboardFinishEdit(bool done);

class NativeBridgeApple
{
public:
    typedef void (*TextChangedCallback)(const char* text);
    typedef void (*FinishEditCallback)(bool done);

    static void ShowNativeKeyboard(const char* text, TextChangedCallback textChangedCallback, FinishEditCallback finishEditCallback);
};

#endif

#endif //RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
