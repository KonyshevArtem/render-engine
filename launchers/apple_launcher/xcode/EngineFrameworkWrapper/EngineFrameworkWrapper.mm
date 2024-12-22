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

struct ImGuiInitData
{
    void* Device;
    void* View;
};

struct ImGuiNewFrameData
{
    void* RenderPassDescriptor;
    void* CommandBuffer;
};

@implementation EngineFrameworkWrapper

+ (void) Initialize:(id<MTLDevice>)device renderCommandBuffer:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer view:(MTKView*)view executablePath:(const char*)executablePath resourcesPath:(const char*)resourcesPath
{
    MetalInitData* metalInitData = new MetalInitData();
    metalInitData->Device = (__bridge void*)device;
    metalInitData->RenderCommandBuffer = (__bridge void*)renderCommandBuffer;
    metalInitData->CopyCommandBuffer = (__bridge void*)copyCommandBuffer;
    
    FileSystemData* fileSystemData = new FileSystemData();
    fileSystemData->ExecutablePath = executablePath;
    fileSystemData->ResourcesPath = resourcesPath;
    
    ImGuiInitData* imGuiInitData = new ImGuiInitData();
    imGuiInitData->Device = (__bridge void*)device;
    imGuiInitData->View = (__bridge void*)view;
    
    EngineFramework::Initialize((void*)fileSystemData, (void*)metalInitData, (void*)imGuiInitData, "Metal");
    
    delete metalInitData;
    delete fileSystemData;
    delete imGuiInitData;
}

+ (void) TickMainLoop:(id<MTLCommandBuffer>)renderCommandBuffer copyCommandBuffer:(id<MTLCommandBuffer>)copyCommandBuffer backbufferDescriptor:(MTLRenderPassDescriptor *)backbufferDescriptor width:(int)width height:(int)height
{
    MetalFrameData* metalFrameData = new MetalFrameData();
    metalFrameData->RenderCommandBuffer = (__bridge void*)renderCommandBuffer;
    metalFrameData->CopyCommandBuffer = (__bridge void*)copyCommandBuffer;
    metalFrameData->BackbufferDescriptor = (__bridge void*)backbufferDescriptor;
    
    ImGuiNewFrameData* imGuiNewFrameData = new ImGuiNewFrameData();
    imGuiNewFrameData->RenderPassDescriptor = (__bridge void*)backbufferDescriptor;
    imGuiNewFrameData->CommandBuffer = (__bridge void*)renderCommandBuffer;
    
    EngineFramework::TickMainLoop(metalFrameData, imGuiNewFrameData, width, height);

    delete metalFrameData;
    delete imGuiNewFrameData;
}

+ (void) Shutdown
{
    EngineFramework::Shutdown();
}

+ (bool) ShouldCloseWindow
{
    return EngineFramework::ShouldCloseWindow();
}

+ (void) ProcessMouseClick:(int)mouseButton pressed:(bool)pressed
{
    EngineFramework::ProcessMouseClick(mouseButton, pressed);
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
