#import "AAPLAppDelegate.h"

@implementation AAPLAppDelegate

#if defined(TARGET_IOS) || defined(TARGET_TVOS)

- (BOOL)application:(PlatformWindow *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    return YES;
}

#else

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

#endif

@end
