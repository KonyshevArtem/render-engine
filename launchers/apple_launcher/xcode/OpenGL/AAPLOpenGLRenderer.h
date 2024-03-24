#if defined(TARGET_IOS) || defined(TARGET_TVOS)
@import UIKit;
#define PlatformViewBase UIView
#else
@import AppKit;
#define PlatformViewBase NSOpenGLView
#endif

#import <Foundation/Foundation.h>
#include "AAPLGLHeaders.h"

@interface AAPLOpenGLRenderer : NSObject

- (instancetype)initWithView:(PlatformViewBase*)view defaultFBOName:(GLuint)defaultFBOName;

- (void)draw;

- (void)resize:(CGSize)size;

@end
