#import "EngineFrameworkWrapper.h"
#import "EngineFramework.h"

@implementation EngineFrameworkWrapper

+ (void) Initialize:(void*)viewPtr graphicsBackend:(NSString*)graphicsBackend
{
    EngineFramework::Initialize(viewPtr, [graphicsBackend UTF8String]);
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

+ (void) ProcessMouseMove:(float)x y:(float)y
{
    EngineFramework::ProcessMouseMove(x, y);
}

+ (void) ProcessKeyPress:(char)key pressed:(bool)pressed
{
    EngineFramework::ProcessKeyPress(key, pressed);
}

@end
