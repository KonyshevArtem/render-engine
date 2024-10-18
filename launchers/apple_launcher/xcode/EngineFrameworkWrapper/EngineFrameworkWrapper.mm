#import "EngineFrameworkWrapper.h"
#import "EngineFramework.h"

struct InitData
{
    void* Device;
    void* CommandBuffer;
};

struct NewFrameData
{
    void* CommandBuffer;
    void* BackbufferDescriptor;
};

@implementation EngineFrameworkWrapper

+ (void) Initialize:(id<MTLDevice>)device commandBuffer:(id<MTLCommandBuffer>)commandBuffer
{
    InitData* data = new InitData();
    data->Device = (__bridge void*)device;
    data->CommandBuffer = (__bridge void*)commandBuffer;
    EngineFramework::Initialize((void*)data, "Metal");
    delete data;
}

+ (void) TickMainLoop:(id<MTLCommandBuffer>)commandBuffer backbufferDescriptor:(MTLRenderPassDescriptor*)backbufferDescriptor width:(int)width height:(int)height
{
    NewFrameData* data = new NewFrameData();
    data->CommandBuffer = (__bridge void*)commandBuffer;
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
