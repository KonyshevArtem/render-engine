#import <MetalKit/MetalKit.h>

/// Platform-independent renderer class.
@interface AAPLMetalRenderer : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;

@end
