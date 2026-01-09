#ifndef RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H
#define RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H

#ifdef RENDER_ENGINE_ANDROID

class NativeBridgeAndroid
{
public:
    static void ShowNativeKeyboard(const char* text, void* textFieldPtr, void* textChangedCallback, void* finishEditCallback);
};

#endif

#endif //RENDER_ENGINE_NATIVE_BRIDGE_ANDROID_H
