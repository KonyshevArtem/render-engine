#import "RenderEngineViewController.h"

#if defined(TARGET_IOS) || defined(TARGET_TVOS)
#define PlatformViewBase UIView
#else
#define PlatformViewBase NSOpenGLView
#endif

@interface AAPLOpenGLView : PlatformViewBase
@end

@interface AAPLOpenGLViewController : RenderEngineViewController
@end
