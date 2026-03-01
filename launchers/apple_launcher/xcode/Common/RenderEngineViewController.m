#import "RenderEngineViewController.h"
#import "EngineFrameworkWrapper.h"
#import "NativeKeyboard.h"
#import <Foundation/Foundation.h>

@implementation RenderEngineViewController

const int k_MouseButtonLeft = 0;
const int k_MouseButtonRight = 1;

NSDictionary *m_SpecialKeys = NULL;

+ (void) initialize
{
#if !defined(TARGET_IOS) && !defined(TARGET_TVOS)
    m_SpecialKeys = @{
        @(NSDeleteFunctionKey): @6,
        @(NSHomeFunctionKey): @7,
        @(NSEndFunctionKey): @8,
        @(NSLeftArrowFunctionKey): @9,
        @(NSRightArrowFunctionKey): @10,
        @(NSUpArrowFunctionKey): @11,
        @(NSDownArrowFunctionKey): @12,
        @('~'): @13 // tilde
    };
#endif
}

- (void) viewDidLoad
{
#if defined(TARGET_IOS)
    [NativeKeyboard Initialize:self.view];
#endif
}

- (CGFloat) getScreenScale
{
#if defined(TARGET_IOS) || defined(TARGET_TVOS)
    return [[UIScreen mainScreen] scale];
#else
    return [[NSScreen mainScreen] backingScaleFactor];;
#endif
}

- (CGSize) getViewSize
{
    CGSize size = [[self view] bounds].size;
    CGFloat scale = [self getScreenScale];
    return CGSizeMake(size.width * scale, size.height * scale);
}

- (CGPoint) scalePoint:(CGPoint)point
{
    CGFloat scale = [self getScreenScale];
    point.x *= scale;
    point.y *= scale;
    return point;
}

#if defined(TARGET_IOS) || defined(TARGET_TVOS)

- (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        CGPoint point = [self scalePoint:[touch locationInView:[self view]]];
        [EngineFrameworkWrapper ProcessTouchDown:[touch hash] x:point.x y:point.y];
    }
}

- (void) touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        CGPoint point = [self scalePoint:[touch locationInView:[self view]]];
        [EngineFrameworkWrapper ProcessTouchMove:[touch hash] x:point.x y:point.y];
    }
}

- (void) touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        [EngineFrameworkWrapper ProcessTouchUp:[touch hash]];
    }
}

- (void) touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        [EngineFrameworkWrapper ProcessTouchUp:[touch hash]];
    }
}

#else

- (void) mouseDown:(NSEvent *)event
{
    [EngineFrameworkWrapper ProcessMouseClick:k_MouseButtonLeft pressed:true];
}

- (void) mouseUp:(NSEvent *)event
{
    [EngineFrameworkWrapper ProcessMouseClick:k_MouseButtonLeft pressed:false];
}

- (void) mouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void) rightMouseDown:(NSEvent *)event
{
    [EngineFrameworkWrapper ProcessMouseClick:k_MouseButtonRight pressed:true];
}

- (void) rightMouseUp:(NSEvent *)event
{
    [EngineFrameworkWrapper ProcessMouseClick:k_MouseButtonRight pressed:false];
}

- (void) rightMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void) otherMouseDown:(NSEvent *)event
{
    [EngineFrameworkWrapper ProcessMouseClick:(int)event.buttonNumber pressed:true];
}

- (void) otherMouseUp:(NSEvent *)event
{
    [EngineFrameworkWrapper ProcessMouseClick:(int)event.buttonNumber pressed:false];
}

- (void) otherMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void) mouseMoved:(NSEvent *)event
{
    CGPoint point = [self scalePoint:event.locationInWindow];
    [EngineFrameworkWrapper ProcessMouseMove:point.x y: [self getViewSize].height - point.y];
}

- (void) keyPress:(NSEvent *)event pressed:(bool)pressed
{
    [EngineFrameworkWrapper ProcessKeyPress:[[event.characters uppercaseString] characterAtIndex:0] pressed:pressed];
}

- (void) specialKeyPress:(int)keyCode pressed:(bool)pressed
{
    [EngineFrameworkWrapper ProcessSpecialKey:keyCode pressed:pressed];
}

- (void) charPress:(NSEvent *)event
{
    const wchar_t* str = (const wchar_t*)[event.characters cStringUsingEncoding:NSUTF32StringEncoding];
    [EngineFrameworkWrapper ProcessCharInput:str[0]];
}

- (void) keyDown:(NSEvent *)event
{
    NSNumber* specialKeyCodePtr = [m_SpecialKeys objectForKey:@([event.characters characterAtIndex:0])];
    if (specialKeyCodePtr != NULL)
        [self specialKeyPress:[specialKeyCodePtr intValue] pressed:true];
    else
    {
        [self keyPress:event pressed:true];
        [self charPress:event];
    }
}

- (void) keyUp:(NSEvent *)event
{
    NSNumber* specialKeyCodePtr = [m_SpecialKeys objectForKey:@([event.characters characterAtIndex:0])];
    if (specialKeyCodePtr != NULL)
        [self specialKeyPress:[specialKeyCodePtr intValue] pressed:false];
    else
        [self keyPress:event pressed:false];
}

#endif

@end
