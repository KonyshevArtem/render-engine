#import "EngineFrameworkWrapper.h"
#import "EngineFramework.h"

struct MetalInitData
{
    void* Device;
    void* RenderCommandBuffer;
    void* CopyCommandBuffer;
};

struct MetalFrameData
{
    void* RenderCommandBuffer;
    void* CopyCommandBuffer;
    void* BackbufferDescriptor;
};

struct FileSystemData
{
    const char* ExecutablePath;
    const char* ResourcesPath;
};

@implementation EngineFrameworkWrapper

+ (void) Initialize:(id<MTLDevice>)device renderCommandBuffer:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer executablePath:(const char*)executablePath resourcesPath:(const char*)resourcesPath
{
    MetalInitData* metalInitData = new MetalInitData();
    metalInitData->Device = (__bridge void*)device;
    metalInitData->RenderCommandBuffer = (__bridge void*)renderCommandBuffer;
    metalInitData->CopyCommandBuffer = (__bridge void*)copyCommandBuffer;
    
    FileSystemData* fileSystemData = new FileSystemData();
    fileSystemData->ExecutablePath = executablePath;
    fileSystemData->ResourcesPath = resourcesPath;
    
    EngineFramework::Initialize((void*)fileSystemData, (void*)metalInitData, "Metal");
    
    delete metalInitData;
    delete fileSystemData;
}

+ (void) TickMainLoop:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer backbufferDescriptor:(MTLRenderPassDescriptor *)backbufferDescriptor width:(int)width height:(int)height
{
    MetalFrameData* data = new MetalFrameData();
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

+ (void) ProcessTouchDown:(unsigned long)touchId x:(float)x y:(float)y
{
    EngineFramework::ProcessTouchDown(touchId, x, y);
}

+ (void) ProcessTouchMove:(unsigned long)touchId x:(float)x y:(float)y
{
    EngineFramework::ProcessTouchMove(touchId, x, y);
}

+ (void) ProcessTouchUp:(unsigned long)touchId
{
    EngineFramework::ProcessTouchUp(touchId);
}

@end
