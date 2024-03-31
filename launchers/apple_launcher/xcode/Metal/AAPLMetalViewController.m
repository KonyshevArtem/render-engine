#import "AAPLMetalViewController.h"
#import "AAPLMetalRenderer.h"

@implementation AAPLMetalView

TrackingAreaProvider *_trackingAreaProvider;

- (void) updateTrackingAreas
{
    if (_trackingAreaProvider == nil)
    {
        _trackingAreaProvider = [TrackingAreaProvider alloc];
    }

    [_trackingAreaProvider updateTrackingArea:self];
}

@end

@implementation AAPLMetalViewController

AAPLMetalView *_view;
AAPLMetalRenderer *_renderer;

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Set the view to use the default device.
    _view = (AAPLMetalView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();

    if(!_view.device)
    {
        assert(!"Metal is not supported on this device.");
        return;
    }

    _renderer = [[AAPLMetalRenderer alloc] initWithMetalKitView:_view];

    if(!_renderer)
    {
        assert(!"Renderer failed initialization.");
        return;
    }

    // Initialize renderer with the view size.
    [_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];

    _view.delegate = _renderer;
}

- (CGSize) getViewSize
{
    return [_renderer getViewSize];
}

@end
