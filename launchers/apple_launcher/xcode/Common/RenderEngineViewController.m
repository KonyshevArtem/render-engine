#import "RenderEngineViewController.h"
#import "EngineFrameworkWrapper.h"
#import <Foundation/Foundation.h>

@implementation RenderEngineViewController

- (CGSize) getViewSize
{
    return CGSizeMake(0, 0);
}

#if !defined(TARGET_IOS) && !defined(TARGET_TVOS)

- (void) mouseMoved:(NSEvent *)event
{
    NSPoint point = event.locationInWindow;
    [EngineFrameworkWrapper ProcessMouseMove:point.x y: [self getViewSize].height - point.y];
}

- (void) keyPress:(NSEvent *)event pressed:(bool)pressed
{
    [EngineFrameworkWrapper ProcessKeyPress:[[event.characters uppercaseString] characterAtIndex:0] pressed:pressed];
}

- (void) keyDown:(NSEvent *)event
{
    [self keyPress:event pressed:true];
}

- (void) keyUp:(NSEvent *)event
{
    [self keyPress:event pressed:false];
}

#endif

@end
