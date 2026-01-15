#ifndef RENDER_ENGINE_NATIVEBRIDGE_H
#define RENDER_ENGINE_NATIVEBRIDGE_H

class NativeBridgeAndroid
{
public:
    static void NativeKeyboardTextChanged(const char *text);
    static void NativeKeyboardFinishEdit(bool done);
};

#endif //RENDER_ENGINE_NATIVEBRIDGE_H
