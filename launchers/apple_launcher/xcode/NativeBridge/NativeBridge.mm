#import <Foundation/Foundation.h>
#import "NativeBridge.h"
#import "NativeKeyboard.h"

void ShowNativeKeyboard(const wchar_t* text)
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
    const wchar_t* textPtr = reinterpret_cast<const wchar_t*>([text cStringUsingEncoding:NSUTF32StringEncoding]);
    NativeKeyboardTextChanged(textPtr);
}

+ (void) NativeKeyboardFinishEdit:(bool)done
{
    NativeKeyboardFinishEdit(done);
}

@end
