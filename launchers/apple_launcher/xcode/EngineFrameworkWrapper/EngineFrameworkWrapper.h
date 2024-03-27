#import <Foundation/Foundation.h>

@interface EngineFrameworkWrapper : NSObject

+ (void) Initialize:(void*)graphicsDevice graphicsBackend:(NSString*)graphicsBackend;
+ (void) TickMainLoop:(void*)commandBufferPtr backbufferDescriptor:(void*)backbufferDescriptor width:(int)widht height:(int)height;
+ (void) Shutdown;
+ (bool) ShouldCloseWindow;

+ (void) ProcessMouseMove:(float) x y:(float)y;
+ (void) ProcessKeyPress:(char)key pressed:(bool)pressed;

@end
