#ifndef RENDER_ENGINE_UI_NATIVE_KEYBOARD_H
#define RENDER_ENGINE_UI_NATIVE_KEYBOARD_H

#include <memory>

class UITextField;

class UINativeKeyboard
{
public:
    static void ShowKeyboard(std::shared_ptr<UITextField> textField);
};

#endif //RENDER_ENGINE_UI_NATIVE_KEYBOARD_H
