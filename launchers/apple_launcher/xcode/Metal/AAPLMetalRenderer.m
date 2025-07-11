#import "AAPLMetalRenderer.h"
#import "EngineFrameworkWrapper.h"

static const MTLPixelFormat AAPLDepthFormat = MTLPixelFormatDepth32Float_Stencil8;
static const MTLPixelFormat AAPLColorFormat = MTLPixelFormatBGRA8Unorm;

/// Main class that performs the rendering.
@implementation AAPLMetalRenderer

bool s_FirstFrame = true;

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

/// Called whenever the view orientation, layout, or size changes.
- (void) mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
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

    if (!s_FirstFrame)
    {
#if defined(TARGET_IOS) || defined(TARGET_TVOS)
        CGSize size = view.drawableSize;
#else
        CGFloat scale = view.window.backingScaleFactor ?: NSScreen.mainScreen.backingScaleFactor;
        CGSize size = view.bounds.size;
        size.width *= scale;
        size.height *= scale;
        view.drawableSize =CGSizeMake(size.width, size.height);
#endif
        
        [EngineFrameworkWrapper TickMainLoop:size.width height:size.height];
    }
    
    s_FirstFrame = false;
}

@end
