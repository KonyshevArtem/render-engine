#ifndef TrackingAreaProvider_h
#define TrackingAreaProvider_h

#if defined(TARGET_IOS) || defined(TARGET_TVOS)
@import UIKit;
#define PlatformView UIView
#else
@import AppKit;
#define PlatformView NSView
#endif

@interface TrackingAreaProvider : NSObject

+ (void) updateTrackingArea:(PlatformView*)view;

@end

#endif /* TrackingAreaProvider_h */
