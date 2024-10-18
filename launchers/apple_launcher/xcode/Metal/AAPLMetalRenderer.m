#import "AAPLMetalRenderer.h"
#import "EngineFrameworkWrapper.h"
#import "ImGuiWrapper.h"

static const MTLPixelFormat AAPLDepthFormat = MTLPixelFormatDepth32Float;
static const MTLPixelFormat AAPLColorFormat = MTLPixelFormatBGRA8Unorm;

/// Main class that performs the rendering.
@implementation AAPLMetalRenderer

id<MTLDevice>        _device;
id<MTLCommandQueue>  _commandQueue;

id<MTLDepthStencilState>   _depthState;

CGSize _viewSize;

/// Initialize the renderer with the MetalKit view that references the Metal device you render with.
/// You also use the MetalKit view to set the pixel format and other properties of the drawable.
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        _device = mtkView.device;
        _commandQueue = [_device newCommandQueue];

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
        
        id<MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
        
        [EngineFrameworkWrapper Initialize:_device commandBuffer:commandBuffer];
        [ImGuiWrapper Init_OSX:mtkView];
        [ImGuiWrapper Init_Metal:_device];
        
        [commandBuffer commit];
    }

    return self;
}

- (void) dealloc
{
    [ImGuiWrapper Shutdown_Metal];
    [ImGuiWrapper Shutdown_OSX];
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
    
    MTLRenderPassDescriptor* renderPassDescriptor = [view currentRenderPassDescriptor];
    id<MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
    
    [ImGuiWrapper NewFrame_Metal:renderPassDescriptor];
    [ImGuiWrapper NewFrame_OSX:view];
    
    [EngineFrameworkWrapper TickMainLoop:commandBuffer backbufferDescriptor:renderPassDescriptor width:_viewSize.width height:_viewSize.height];
    
    [[[renderPassDescriptor colorAttachments] objectAtIndexedSubscript:0] setLoadAction:MTLLoadActionLoad];
    id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    [ImGuiWrapper Render_Metal:commandBuffer commandEncoder:commandEncoder];
    [commandEncoder endEncoding];
    
    [commandBuffer presentDrawable:view.currentDrawable];
    [commandBuffer commit];
}

- (CGSize) getViewSize
{
    return _viewSize;
}

@end
