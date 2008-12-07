#import <Cocoa/Cocoa.h>

@interface GameDialogController : NSWindowController {
    IBOutlet NSImageView *mapPreview;
	
	bool startGame;
	
	int initialFunds;
	int fundsPerBuilding;
	NSString* mapFile;
}

@property(readwrite) int initialFunds;
@property(readwrite) int fundsPerBuilding;
@property(assign) NSString* mapFile;

-(GameDialogController*)init;
- (bool)run;

- (IBAction)cancel:(id)sender;
- (IBAction)chooseMap:(id)sender;
- (IBAction)startGame:(id)sender;
@end
