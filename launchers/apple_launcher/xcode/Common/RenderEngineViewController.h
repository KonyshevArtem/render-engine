#ifndef RenderEngineViewController_h
#define RenderEngineViewController_h

#if defined(TARGET_IOS) || defined(TARGET_TVOS)
@import UIKit;
#define PlatformViewController UIViewController
#else
@import AppKit;
#define PlatformViewController NSViewController
#endif

@interface RenderEngineViewController : PlatformViewController

- (CGSize) getViewSize;

@end

@interface TrackingAreaProvider : NSObject

- (void) updateTrackingArea:(NSView*)view;

@end

#endif /* RenderEngineViewController_h */
