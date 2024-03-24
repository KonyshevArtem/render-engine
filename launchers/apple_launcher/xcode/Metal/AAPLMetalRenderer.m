@import MetalKit;

#import "AAPLMetalRenderer.h"
#import "EngineFrameworkWrapper.h"

static const MTLPixelFormat AAPLDepthFormat = MTLPixelFormatDepth32Float;
static const MTLPixelFormat AAPLColorFormat = MTLPixelFormatBGRA8Unorm_sRGB;

/// Main class that performs the rendering.
@implementation AAPLMetalRenderer
{
    id<MTLDevice>        _device;
    id<MTLCommandQueue>  _commandQueue;

    id<MTLDepthStencilState>   _depthState;
    
    CGSize _viewSize;
}

/// Initialize the renderer with the MetalKit view that references the Metal device you render with.
/// You also use the MetalKit view to set the pixel format and other properties of the drawable.
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        _device = mtkView.device;

        mtkView.colorPixelFormat        = AAPLColorFormat;
        mtkView.depthStencilPixelFormat = AAPLDepthFormat;
        {
            // Configure a combined depth and stencil descriptor that enables the creation
            // of an immutable depth and stencil state object.
            MTLDepthStencilDescriptor *depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
            depthStencilDesc.depthCompareFunction = MTLCompareFunctionLess;
            depthStencilDesc.depthWriteEnabled = YES;
            _depthState = [_device newDepthStencilStateWithDescriptor:depthStencilDesc];
        }
        
        [EngineFrameworkWrapper Initialize:(void*)mtkView graphicsBackend:@"Metal"];
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
    _viewSize = size;
}

/// Called whenever the view needs to render.
- (void) drawInMTKView:(nonnull MTKView *)view
{
    if ([EngineFrameworkWrapper ShouldCloseWindow])
    {
        [[view window] performClose:self];
        return;
    }
    
    [EngineFrameworkWrapper TickMainLoop:_viewSize.width height:_viewSize.height];
}

@end
