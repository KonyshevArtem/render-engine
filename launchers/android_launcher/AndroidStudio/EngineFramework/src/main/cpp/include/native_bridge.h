#ifndef RENDER_ENGINE_NATIVEBRIDGE_H
#define RENDER_ENGINE_NATIVEBRIDGE_H

#include <cstddef>

class NativeBridgeAndroid
{
public:
    static void NativeKeyboardTextChanged(const char16_t* text, size_t length);
    static void NativeKeyboardFinishEdit(bool done);
};

#endif //RENDER_ENGINE_NATIVEBRIDGE_H
