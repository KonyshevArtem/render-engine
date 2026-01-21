#import "NativeKeyboard.h"
#import "NativeBridge.h"

#if defined(TARGET_IOS)

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <wchar.h>

@interface KeyboardTextFieldDelegate : NSObject<UITextFieldDelegate>
@end

@implementation KeyboardTextFieldDelegate

- (BOOL) textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return TRUE;
}

- (void) textFieldDidEndEditing:(UITextField *)textField reason:(UITextFieldDidEndEditingReason)reason
{
    [NativeBridge NativeKeyboardFinishEdit:reason == UITextFieldDidEndEditingReasonCommitted ? TRUE : FALSE];
    [textField setHidden:TRUE];
}

@end

@implementation NativeKeyboard

UITextField* s_TextField;
KeyboardTextFieldDelegate* s_TextFieldDelegate;

+ (void) Initialize:(UIView*)view
{
    s_TextField = [[UITextField alloc] init];
    s_TextFieldDelegate = [[KeyboardTextFieldDelegate alloc] init];
    
    [s_TextField setDelegate:s_TextFieldDelegate];
    [s_TextField setBorderStyle:UITextBorderStyleRoundedRect];
    [s_TextField setTranslatesAutoresizingMaskIntoConstraints:FALSE];
    [s_TextField setHidden:TRUE];
    
    [view addSubview:s_TextField];
    
    NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
    [nc addObserver:self selector:@selector(OnTextChanged:) name:UITextFieldTextDidChangeNotification object:s_TextField];
    
    UILayoutGuide* safeAreaGuide = [view safeAreaLayoutGuide];
    UILayoutGuide* keyboardGuide = [view keyboardLayoutGuide];
    [NSLayoutConstraint activateConstraints:@[
        [s_TextField.leadingAnchor constraintEqualToAnchor:safeAreaGuide.leadingAnchor],
        [s_TextField.trailingAnchor constraintEqualToAnchor:safeAreaGuide.trailingAnchor],
        [s_TextField.bottomAnchor constraintEqualToAnchor:keyboardGuide.topAnchor],
        [s_TextField.heightAnchor constraintLessThanOrEqualToConstant:40]
    ]];
}

+ (void) Show:(const wchar_t*)text
{
    NSString* textStr = [[NSString alloc] initWithBytes:text length:wcslen(text)*sizeof(*text) encoding:NSUTF32LittleEndianStringEncoding];
    
    [s_TextField setHidden:FALSE];
    [s_TextField setText:textStr];
    [s_TextField becomeFirstResponder];
}

+ (void) OnTextChanged:(id)notification {

    [NativeBridge NativeKeyboardTextChanged:[s_TextField text]];
}

@end

#endif
