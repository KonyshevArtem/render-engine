#import "EngineFrameworkWrapper.h"
#import "EngineFramework.h"

struct FileSystemData
{
    const char* ExecutablePath;
    const char* ResourcesPath;
};

@implementation EngineFrameworkWrapper

+ (void) Initialize:(MTKView*)view executablePath:(const char*)executablePath resourcesPath:(const char*)resourcesPath
{
    FileSystemData* fileSystemData = new FileSystemData();
    fileSystemData->ExecutablePath = executablePath;
    fileSystemData->ResourcesPath = resourcesPath;
    
    EngineFramework::Initialize((void*)fileSystemData, (__bridge void*)view, "Metal");
    
    delete fileSystemData;
}

+ (void) TickMainLoop:(int)width height:(int)height
{
    EngineFramework::TickMainLoop(width, height);
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
