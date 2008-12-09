#import <Cocoa/Cocoa.h>

#include "game/player.h"

@interface BuyMenuController : NSWindowController {
	IBOutlet NSTableView* tableView;
	
	IBOutlet NSButton* buyButton;

	NSMutableArray* unitArray;
	
	aw::player::ptr player;
	aw::unit::workshops workshop;
	
	aw::unit::types returnValue;
}

@property(readonly) int playerFunds;

-(BuyMenuController*)initWithPlayer:(aw::player::ptr)player workshop:(aw::unit::workshops)ws;
-(aw::unit::types)run;

-(IBAction)buyButtonClicked:(NSButton*)button;
-(IBAction)cancelButtonClicked:(NSButton*)button;

@end
