#import <Cocoa/Cocoa.h>

@interface GameDialogController : NSWindowController {
    IBOutlet NSImageView *mapPreview;
	
	bool startGame;
	
	bool mapIsValid;
	
	int initialFunds;
	int fundsPerBuilding;
	NSString* mapFile;
}

@property int initialFunds;
@property int fundsPerBuilding;
@property(retain) NSString* mapFile;
@property bool mapIsValid;

-(GameDialogController*)init;
- (bool)run;

- (IBAction)cancel:(id)sender;
- (IBAction)chooseMap:(id)sender;
- (IBAction)startGame:(id)sender;

- (NSImage*)createMapPreview;
@end
