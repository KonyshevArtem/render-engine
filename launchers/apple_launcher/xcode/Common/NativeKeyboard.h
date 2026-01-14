#if defined(TARGET_IOS)

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface NativeKeyboard : NSObject

+ (void) Initialize:(UIView*)view;
+ (void) Show:(const char*)text;

@end

#endif
