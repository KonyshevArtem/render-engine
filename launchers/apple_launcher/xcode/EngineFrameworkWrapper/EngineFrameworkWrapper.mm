#import "EngineFrameworkWrapper.h"
#import "EngineFramework.h"

struct InitData
{
    void* Device;
    void* RenderCommandBuffer;
    void* CopyCommandBuffer;
};

struct NewFrameData
{
    void* RenderCommandBuffer;
    void* CopyCommandBuffer;
    void* BackbufferDescriptor;
};

@implementation EngineFrameworkWrapper

+ (void) Initialize:(id<MTLDevice>)device renderCommandBuffer:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer
{
    InitData* data = new InitData();
    data->Device = (__bridge void*)device;
    data->RenderCommandBuffer = (__bridge void*)renderCommandBuffer;
    data->CopyCommandBuffer = (__bridge void*)copyCommandBuffer;
    EngineFramework::Initialize((void*)data, "Metal");
    delete data;
}

+ (void) TickMainLoop:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer backbufferDescriptor:(MTLRenderPassDescriptor *)backbufferDescriptor width:(int)width height:(int)height
{
    NewFrameData* data = new NewFrameData();
    data->RenderCommandBuffer = (__bridge void*)renderCommandBuffer;
    data->CopyCommandBuffer = (__bridge void*)copyCommandBuffer;
    data->BackbufferDescriptor = (__bridge void*)backbufferDescriptor;
    EngineFramework::TickMainLoop(data, width, height);
    delete data;
}

+ (void) Shutdown
{
    EngineFramework::Shutdown();
}

+ (bool) ShouldCloseWindow
{
    return EngineFramework::ShouldCloseWindow();
}

+ (void) ProcessMouseMove:(float)x y:(float)y
{
    EngineFramework::ProcessMouseMove(x, y);
}

+ (void) ProcessKeyPress:(char)key pressed:(bool)pressed
{
    EngineFramework::ProcessKeyPress(key, pressed);
}

@end
