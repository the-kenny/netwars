#import <Cocoa/Cocoa.h>

#import "MapView.h"

#include "game/gui/main_window.h"

@interface MainWindowController : NSWindowController {
    MapView* mapView;
	IBOutlet NSView* mainView;
	
	NSViewController* mapViewController;
}

@property(readonly) MapView* mapView;
@property(readonly) NSViewController* mapViewController;

@end
