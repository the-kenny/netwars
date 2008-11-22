#import <Cocoa/Cocoa.h>

#import "MapView.h"

@interface MainWindowController : NSWindowController {
    IBOutlet id mapView;
	
	NSViewController* mapViewController;
}

@property(readonly) NSViewController* mapViewController;

@end
