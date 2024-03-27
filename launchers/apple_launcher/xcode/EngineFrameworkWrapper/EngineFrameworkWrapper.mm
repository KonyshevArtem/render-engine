#import "EngineFrameworkWrapper.h"
#import "EngineFramework.h"

@implementation EngineFrameworkWrapper

+ (void) Initialize:(void*)graphicsDevice graphicsBackend:(NSString*)graphicsBackend
{
    EngineFramework::Initialize(graphicsDevice, [graphicsBackend UTF8String]);
}

+ (void) TickMainLoop:(void*)commandBufferPtr backbufferDescriptor:(void*)backbufferDescriptor width:(int)width height:(int)height
{
    EngineFramework::TickMainLoop(commandBufferPtr, backbufferDescriptor, width, height);
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
