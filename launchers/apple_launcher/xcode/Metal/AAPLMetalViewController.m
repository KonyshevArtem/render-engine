#import "AAPLMetalViewController.h"
#import "AAPLMetalRenderer.h"
#import "TrackingAreaProvider.h"

@implementation AAPLMetalView

- (void) updateTrackingAreas
{
    [TrackingAreaProvider updateTrackingArea:self];
}

@end

@implementation AAPLMetalViewController

AAPLMetalView *_view;
AAPLMetalRenderer *_renderer;

- (void)viewDidLoad
{
    [super viewDidLoad];

    _view = (AAPLMetalView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();

#if !defined(TARGET_IOS) && !defined(TARGET_TVOS)
    [_view setFrameSize:NSSizeFromCGSize(CGSizeMake(1920, 1080))];
#endif

    _renderer = [[AAPLMetalRenderer alloc] initWithMetalKitView:_view];
    [_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];

    _view.delegate = _renderer;
}

@end
