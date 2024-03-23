#if defined(TARGET_IOS) || defined(TARGET_TVOS)
@import UIKit;
#define PlatformViewBase UIView
#define PlatformViewController UIViewController
#else
@import AppKit;
#define PlatformViewBase NSOpenGLView
#define PlatformViewController NSViewController
#endif

@interface AAPLOpenGLView : PlatformViewBase

- (void) updateTrackingAreas;
- (void) closeWindow;

@end

@interface AAPLOpenGLViewController : PlatformViewController

- (void) mouseMoved:(NSEvent *)event;
- (void) keyDown:(NSEvent *)event;
- (void) keyUp:(NSEvent *)event;

@end
