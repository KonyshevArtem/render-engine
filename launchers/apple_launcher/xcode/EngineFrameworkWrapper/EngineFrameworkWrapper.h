#import <Metal/MTLDevice.h>
#import <Metal/MTLCommandBuffer.h>
#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

@interface EngineFrameworkWrapper : NSObject

+ (void) Initialize:(MTKView*)view executablePath:(const char*)executablePath resourcesPath:(const char*)resourcesPath;
+ (void) TickMainLoop:(int)width height:(int)height;
+ (void) Shutdown;
+ (bool) ShouldCloseWindow;

+ (void) ProcessMouseClick:(int)mouseButton pressed:(bool)pressed;
+ (void) ProcessMouseMove:(float)x y:(float)y;
+ (void) ProcessKeyPress:(char)key pressed:(bool)pressed;

+ (void) ProcessTouchDown:(unsigned long)touchId x:(float)x y:(float)y;
+ (void) ProcessTouchMove:(unsigned long)touchId x:(float)x y:(float)y;
+ (void) ProcessTouchUp:(unsigned long)touchId;

@end
