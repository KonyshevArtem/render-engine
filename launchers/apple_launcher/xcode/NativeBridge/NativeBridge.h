#ifndef RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
#define RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H

#include <Foundation/Foundation.h>

typedef void (*ShowNativeKeyboardCallback)(const char* text);

void SetShowNativeKeyboardCallback(ShowNativeKeyboardCallback fn);

void NativeKeyboardTextChanged(const char* text);
void NativeKeyboardFinishEdit(bool done);

@interface NativeBridge : NSObject

+ (void) InitNativeBridge;
+ (void) NativeKeyboardTextChanged:(NSString*)text;
+ (void) NativeKeyboardFinishEdit:(bool)done;

@end

#endif //RENDER_ENGINE_NATIVE_BRIDGE_APPLE_H
