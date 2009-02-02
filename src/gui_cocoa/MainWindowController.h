#import <Cocoa/Cocoa.h>

#import "MapView.h"
#import "MapViewController.h"

#include "game/gui/main_window.h"

@interface MainWindowController : NSWindowController {
    MapView* mapView;
	IBOutlet NSView* mainView;
	
	MapViewController* mapViewController;
}

@property(readonly) MapView* mapView;
@property(readonly) MapViewController* mapViewController;

@end
