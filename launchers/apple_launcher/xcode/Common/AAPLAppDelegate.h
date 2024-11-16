#if defined(TARGET_IOS) || defined(TARGET_TVOS)
#import <UIKit/UIKit.h>
#define PlatformWindow UIWindow
#define PlatformApplication UIApplication
#define PlatformApplicationDelegate UIResponder<UIApplicationDelegate>
#else
#import <AppKit/AppKit.h>
#define PlatformWindow NSWindow
#define PlatformApplication NSApplication
#define PlatformApplicationDelegate NSObject<NSApplicationDelegate>
#endif

@interface AAPLAppDelegate : PlatformApplicationDelegate

@property (strong, nonatomic) PlatformWindow *window;

- (BOOL) application:(PlatformApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(PlatformApplication *)sender;

@end
