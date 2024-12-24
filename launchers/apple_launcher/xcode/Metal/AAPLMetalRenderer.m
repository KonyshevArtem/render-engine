#import "AAPLMetalRenderer.h"
#import "EngineFrameworkWrapper.h"

static const MTLPixelFormat AAPLDepthFormat = MTLPixelFormatDepth32Float_Stencil8;
static const MTLPixelFormat AAPLColorFormat = MTLPixelFormatBGRA8Unorm;

/// Main class that performs the rendering.
@implementation AAPLMetalRenderer

CGSize s_ViewSize;

/// Initialize the renderer with the MetalKit view that references the Metal device you render with.
/// You also use the MetalKit view to set the pixel format and other properties of the drawable.
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        mtkView.colorPixelFormat = AAPLColorFormat;
        mtkView.depthStencilPixelFormat = AAPLDepthFormat;
        
        [EngineFrameworkWrapper Initialize:mtkView];
    }

    return self;
}

- (void) dealloc
{
    [EngineFrameworkWrapper Shutdown];
}

/// Called whenever the view orientation, layout, or size changes.
- (void) mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    s_ViewSize = size;
}

/// Called whenever the view needs to render.
- (void) drawInMTKView:(nonnull MTKView *)view
{
#if !defined(TARGET_IOS) && !defined(TARGET_TVOS)
    if ([EngineFrameworkWrapper ShouldCloseWindow])
    {
        [[view window] performClose:self];
        return;
    }
#endif
    
    [EngineFrameworkWrapper TickMainLoop:s_ViewSize.width height:s_ViewSize.height];
}

@end
