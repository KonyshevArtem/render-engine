#import <Metal/MTLDevice.h>
#import <Metal/MTLCommandBuffer.h>
#import <Foundation/Foundation.h>

@interface EngineFrameworkWrapper : NSObject

+ (void) Initialize:(id<MTLDevice>)device commandBuffer:(id<MTLCommandBuffer>)commandBuffer;
+ (void) TickMainLoop:(id<MTLCommandBuffer>)commandBuffer backbufferDescriptor:(MTLRenderPassDescriptor*)backbufferDescriptor width:(int)widht height:(int)height;
+ (void) Shutdown;
+ (bool) ShouldCloseWindow;

+ (void) ProcessMouseMove:(float) x y:(float)y;
+ (void) ProcessKeyPress:(char)key pressed:(bool)pressed;

@end
