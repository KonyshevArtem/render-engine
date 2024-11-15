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
@end

#endif /* RenderEngineViewController_h */
