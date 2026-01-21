#ifndef RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
#define RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H

#include <Foundation/Foundation.h>

typedef void (*ShowNativeKeyboardCallback)(const wchar_t* text);

void SetShowNativeKeyboardCallback(ShowNativeKeyboardCallback fn);

void NativeKeyboardTextChanged(const wchar_t* text);
void NativeKeyboardFinishEdit(bool done);

@interface NativeBridge : NSObject

+ (void) InitNativeBridge;
+ (void) NativeKeyboardTextChanged:(NSString*)text;
+ (void) NativeKeyboardFinishEdit:(bool)done;

@end

#endif //RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
