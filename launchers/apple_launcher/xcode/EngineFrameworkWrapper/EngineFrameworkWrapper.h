#import <Foundation/Foundation.h>

@interface EngineFrameworkWrapper : NSObject

+ (void) Initialize:(void*)viewPtr;
+ (void) TickMainLoop:(int)widht height:(int)height;
+ (void) Shutdown;
+ (bool) ShouldCloseWindow;

+ (void) ProcessMouseMove:(float) x y:(float)y;
+ (void) ProcessKeyPress:(char)key pressed:(bool)pressed;

@end
