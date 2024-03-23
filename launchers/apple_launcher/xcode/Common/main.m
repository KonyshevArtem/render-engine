#if defined(TARGET_IOS) || defined(TARGET_TVOS)
#import <UIKit/UIKit.h>
#import <TargetConditionals.h>
#import <Availability.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#import "AAPLAppDelegate.h"

#if defined(TARGET_IOS) || defined(TARGET_TVOS)

int main(int argc, char * argv[]) {

#if TARGET_OS_SIMULATOR && (!defined(__IPHONE_13_0) ||  !defined(__TVOS_13_0))
#error Metal is not supported in this version of Simulator.
#endif

    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AAPLAppDelegate class]));
    }
}

#elif defined(TARGET_MACOS)

int main(int argc, const char * argv[]) {
    NSApplication* app = [NSApplication sharedApplication];
    [app setDelegate:[AAPLAppDelegate alloc]];
    
    return NSApplicationMain(argc, argv);
}

#endif
