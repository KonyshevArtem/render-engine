#import "TrackingAreaProvider.h"
#import <Foundation/Foundation.h>

@implementation TrackingAreaProvider

#if !defined(TARGET_IOS) && !defined(TARGET_TVOS)

NSTrackingArea* s_TrackingArea;

+ (void) updateTrackingArea:(PlatformView *)view
{
    if (s_TrackingArea != nil)
    {
        [view removeTrackingArea:s_TrackingArea];
    }
    
    NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow;
    s_TrackingArea = [[NSTrackingArea alloc] initWithRect:[view bounds] options:options owner:view userInfo:nil];
    [view addTrackingArea:s_TrackingArea];
}

#else

+ (void) updateTrackingArea:(PlatformView *)view {}

#endif

@end
