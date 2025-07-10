#import "AAPLAppDelegate.h"
#import "EngineFrameworkWrapper.h"

@implementation AAPLAppDelegate

- (BOOL) application:(PlatformApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    return YES;
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(PlatformApplication *)sender
{
    return YES;
}

- (void) applicationWillTerminate:(NSNotification *)notification
{
    [EngineFrameworkWrapper Shutdown];
}

@end
