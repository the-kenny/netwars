#import "MainWindowController.h"

@implementation MainWindowController
- (void)windowDidLoad {
	NSLog(@"MainWindowController.windowDidLoad");
	
	[self.window setAcceptsMouseMovedEvents:YES];
	
	mapViewController = [[NSViewController alloc] initWithNibName:@"MapView" bundle:nil];
	[mapView addSubview:mapViewController.view];
}

@synthesize mapViewController;

@end
