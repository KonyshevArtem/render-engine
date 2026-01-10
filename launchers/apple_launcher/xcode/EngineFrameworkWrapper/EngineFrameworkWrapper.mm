#import "EngineFrameworkWrapper.h"
#import "EngineFramework.h"

@implementation EngineFrameworkWrapper

+ (void) Initialize:(MTKView*)view
{
    char* argv = "-metal";
    EngineFramework::Initialize(nullptr, (__bridge void*)view, &argv, 1);
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

+ (void) ProcessCharInput:(unsigned char) ch;
{
    EngineFramework::ProcessCharInput(ch);
}

+ (void) ProcessSpecialKey:(int)keyId  pressed:(bool)pressed
{
    EngineFramework::ProcessSpecialKey(keyId, pressed);
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
