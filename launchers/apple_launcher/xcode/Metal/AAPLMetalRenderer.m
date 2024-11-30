#import "AAPLMetalRenderer.h"
#import "EngineFrameworkWrapper.h"
#import "ImGuiWrapper.h"

static const MTLPixelFormat AAPLDepthFormat = MTLPixelFormatDepth32Float_Stencil8;
static const MTLPixelFormat AAPLColorFormat = MTLPixelFormatBGRA8Unorm;

/// Main class that performs the rendering.
@implementation AAPLMetalRenderer

id<MTLDevice> s_Device;
id<MTLCommandQueue> s_RenderCommandQueue;
id<MTLCommandQueue> s_CopyCommandQueue;

CGSize s_ViewSize;

/// Initialize the renderer with the MetalKit view that references the Metal device you render with.
/// You also use the MetalKit view to set the pixel format and other properties of the drawable.
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        s_Device = mtkView.device;
        s_RenderCommandQueue = [s_Device newCommandQueue];
        s_CopyCommandQueue = [s_Device newCommandQueue];
        
        [s_RenderCommandQueue setLabel:@"Render Queue"];
        [s_CopyCommandQueue setLabel:@"Copy Queue"];

        mtkView.colorPixelFormat = AAPLColorFormat;
        mtkView.depthStencilPixelFormat = AAPLDepthFormat;
        
        id<MTLCommandBuffer> renderCommandBuffer = [s_RenderCommandQueue commandBuffer];
        id<MTLCommandBuffer> copyCommandBuffer = [s_CopyCommandQueue commandBuffer];
        
        NSString* executablePath = [[NSBundle mainBundle] executablePath];
        NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
        
        [EngineFrameworkWrapper Initialize:s_Device renderCommandBuffer:renderCommandBuffer copyCommandBuffer:copyCommandBuffer executablePath:[executablePath UTF8String] resourcesPath:[resourcePath UTF8String]];
        [ImGuiWrapper Init_OSX:mtkView];
        [ImGuiWrapper Init_Metal:s_Device];
        
        [copyCommandBuffer commit];
        [renderCommandBuffer commit];
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
    
    MTLRenderPassDescriptor* renderPassDescriptor = [view currentRenderPassDescriptor];
    id<MTLCommandBuffer> renderCommandBuffer = [s_RenderCommandQueue commandBuffer];
    id<MTLCommandBuffer> copyCommandBuffer = [s_CopyCommandQueue commandBuffer];
    
    [ImGuiWrapper NewFrame_Metal:renderPassDescriptor];
    [ImGuiWrapper NewFrame_OSX:view];
    
    [EngineFrameworkWrapper TickMainLoop:renderCommandBuffer copyCommandBuffer:copyCommandBuffer backbufferDescriptor:renderPassDescriptor width:s_ViewSize.width height:s_ViewSize.height];
    
    [[[renderPassDescriptor colorAttachments] objectAtIndexedSubscript:0] setLoadAction:MTLLoadActionLoad];
    id<MTLRenderCommandEncoder> commandEncoder = [renderCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    [ImGuiWrapper Render_Metal:renderCommandBuffer commandEncoder:commandEncoder];
    [commandEncoder endEncoding];
    
    [copyCommandBuffer commit];
    [renderCommandBuffer presentDrawable:view.currentDrawable];
    [renderCommandBuffer commit];
}

@end
