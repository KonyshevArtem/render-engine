#import <Foundation/Foundation.h>
#import "NativeBridge.h"
#import "NativeKeyboard.h"

void ShowNativeKeyboard(const char* text)
{
#if defined(TARGET_IOS)
    [NativeKeyboard Show:text];
#endif
}

@implementation NativeBridge

+ (void) InitNativeBridge
{
    SetShowNativeKeyboardCallback(ShowNativeKeyboard);
}

+ (void) NativeKeyboardTextChanged:(NSString *)text
{
    NativeKeyboardTextChanged([text cStringUsingEncoding:NSUTF8StringEncoding]);
}

+ (void) NativeKeyboardFinishEdit:(bool)done
{
    NativeKeyboardFinishEdit(done);
}

@end
