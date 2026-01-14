#import "AAPLAppDelegate.h"
#import "EngineFrameworkWrapper.h"
#import "NativeBridge.h"

@implementation AAPLAppDelegate

#if defined(TARGET_IOS) || defined(TARGET_TVOS)
- (BOOL) application:(PlatformApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [NativeBridge InitNativeBridge];
    return YES;
}
#else
- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    [NativeBridge InitNativeBridge];
}
#endif

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(PlatformApplication *)sender
{
    return YES;
}

- (void) applicationWillTerminate:(NSNotification *)notification
{
    [EngineFrameworkWrapper Shutdown];
}

@end
