#import <Metal/MTLDevice.h>
#import <Metal/MTLCommandBuffer.h>
#import <Foundation/Foundation.h>

@interface EngineFrameworkWrapper : NSObject

+ (void) Initialize:(id<MTLDevice>)device renderCommandBuffer:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer executablePath:(const char*)executablePath resourcesPath:(const char*)resourcesPath;
+ (void) TickMainLoop:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer backbufferDescriptor:(MTLRenderPassDescriptor*)backbufferDescriptor width:(int)width height:(int)height;
+ (void) Shutdown;
+ (bool) ShouldCloseWindow;

+ (void) ProcessMouseMove:(float) x y:(float)y;
+ (void) ProcessKeyPress:(char)key pressed:(bool)pressed;

@end
