#import <Cocoa/Cocoa.h>
#import "MapView.h"
#import "MainWindowController.h"
#import "CocoaMapWidget.h"

#import "game/game.h"
#import "game/game_controller.h"

@interface AppController : NSObject {
	MainWindowController *mainWindowController;
    //NSViewController* mapViewController;
	
	MapView* mapView;
	
	CocoaMapWidget::ptr cocoaMapWidget;
	aw::game_controller::ptr gameController;
	
	bool gameActive;
}

@property(readwrite) bool gameActive;

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename;

- (IBAction)startNewGame:(id)sender;
- (IBAction)endTurn:(id)sender;
- (void)gameFinished:(const aw::player::ptr&)winner;

@end
