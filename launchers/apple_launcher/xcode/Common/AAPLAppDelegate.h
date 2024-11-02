#if defined(TARGET_IOS) || defined(TARGET_TVOS)
#import <UIKit/UIKit.h>
#define PlatformApplicationDelegate UIResponder<UIApplicationDelegate>
#else
#import <AppKit/AppKit.h>
#define PlatformApplicationDelegate NSObject<NSApplicationDelegate>
#endif

@interface AAPLAppDelegate : PlatformApplicationDelegate

#if defined(TARGET_IOS) || defined(TARGET_TVOS)

@property (strong, nonatomic) UIWindow *window;

#else

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;

#endif

@end
