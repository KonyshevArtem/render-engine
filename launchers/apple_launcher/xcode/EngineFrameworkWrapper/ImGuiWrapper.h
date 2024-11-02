#ifndef ImGuiWrapper_h
#define ImGuiWrapper_h

#import <AppKit/AppKit.h>
#import <MetalKit/MetalKit.h>
#import <Foundation/Foundation.h>

@interface ImGuiWrapper : NSObject

+ (bool) Init_Metal:(id<MTLDevice>_Nonnull)device;
+ (void) Shutdown_Metal;
+ (void) NewFrame_Metal:(MTLRenderPassDescriptor*_Nonnull)renderPassDescriptor;
+ (void) Render_Metal:(id<MTLCommandBuffer>_Nonnull)commandBuffer commandEncoder:(id<MTLRenderCommandEncoder>_Nonnull) commandEncoder;

+ (bool) Init_OSX:(NSView* _Nonnull) view;
+ (void) Shutdown_OSX;
+ (void) NewFrame_OSX:(NSView* _Nullable) view;

@end

#endif /* ImGuiWrapper_h */
