#import <Cocoa/Cocoa.h>

#include "game/game_settings.h"

@interface GameDialogController : NSWindowController {
    IBOutlet NSImageView *mapPreview;
	
	bool startGame;
	
	int initialFunds;
	int fundsPerBuilding;
	NSString* mapFile;
}

@property int initialFunds;
@property int fundsPerBuilding;
@property(retain) NSString* mapFile;
@property(readonly) aw::game_settings gameSettings;

-(GameDialogController*)init;
- (bool)run;

- (IBAction)cancel:(id)sender;
- (IBAction)chooseMap:(id)sender;
- (IBAction)startGame:(id)sender;

- (NSImage*)createMapPreview;
@end
