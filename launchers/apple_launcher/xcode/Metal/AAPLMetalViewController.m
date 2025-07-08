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

    _renderer = [[AAPLMetalRenderer alloc] initWithMetalKitView:_view];
    _view.delegate = _renderer;
}

@end
