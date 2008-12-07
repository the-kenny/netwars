#import "GameDialogController.h"

@implementation GameDialogController

@synthesize initialFunds;
@synthesize fundsPerBuilding;
@synthesize mapFile;

-(GameDialogController*)init {
	self = [super initWithWindowNibName:@"GameDialog"];

	startGame = false;
	
	self.initialFunds = 1000;
	self.fundsPerBuilding = 1000;
	
	return self;
}

- (bool)run {
	[self showWindow:self];
	[[NSApplication sharedApplication] runModalForWindow:self.window];
	[self close];
	
	return startGame;
}

- (IBAction)cancel:(id)sender {
	self.initialFunds = 0;
	self.fundsPerBuilding = 0;
	self.mapFile = @"";
	
	startGame = false;
	
    [[NSApplication sharedApplication] stopModal];
}

- (IBAction)chooseMap:(id)sender {
    NSOpenPanel* panel = [NSOpenPanel openPanel]; 
	[panel setCanChooseDirectories:NO]; 
	[panel setCanChooseFiles:YES]; 
	[panel setCanCreateDirectories:YES]; 
	[panel setAllowsMultipleSelection:NO]; 
	
	NSArray* fileTypes = [NSArray arrayWithObjects:@"aws", @"awd", @"awm", @"aw2", nil];
	
	[panel setTitle:@"Select a map to open"]; // Display the dialog. If the OK button was pressed, // process the files. 
	if ([panel runModalForDirectory:nil file:nil types:fileTypes] == NSOKButton) { 
		self.mapFile = [[panel filenames] objectAtIndex:0];
	}
}
	
- (IBAction)startGame:(id)sender {
	startGame = true;
	[[NSApplication sharedApplication] stopModal];
}

#pragma mark Window Delegate Methods

- (void)windowWillClose:(NSNotification *)notification {
	[[NSApplication sharedApplication] stopModal];
}

@end
