@import MetalKit;

/// Platform-independent renderer class.
@interface AAPLMetalRenderer : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;

- (CGSize) getViewSize;

@end
