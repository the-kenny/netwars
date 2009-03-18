#import "GameDialogController.h"

#import "QuartzCore/QuartzCore.h"

#include "game/config.h"
#include "game/map_loader/map_loader.h"

@implementation GameDialogController

@synthesize initialFunds;
@synthesize fundsPerBuilding;
@synthesize mapFile;

-(GameDialogController*)init {
	self = [super initWithWindowNibName:@"GameDialog"];

	startGame = false;
	
	self.initialFunds = aw::config().get<int>("/config/defaults/initial-funds");
	self.fundsPerBuilding = aw::config().get<int>("/config/defaults/funds-per-building");
	
	NSString* mapPath = [[NSBundle mainBundle] pathForResource:@"7330" ofType:@"aws"];
	if(mapPath != nil)
		self.mapFile = mapPath;
		
	return self;
}

-(void)dealloc {
	[mapFile release];
	[super dealloc];
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

		
		[NSThread detachNewThreadSelector:@selector(createMapPreviewThreaded) toTarget:self withObject:nil];
		//[thread start];
		//[self createMapPreview];
	}
}
	
- (IBAction)startGame:(id)sender {
	startGame = true;
	[[NSApplication sharedApplication] stopModal];
}

- (void)createMapPreviewThreaded {
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	[mapPreview performSelectorOnMainThread:@selector(setImage:) 
								 withObject:[self createMapPreview] 
							  waitUntilDone:NO];
	
	[pool release];
}

- (NSImage*)createMapPreview {
	aw::map_loader m([mapFile UTF8String]);
	aw::map_loader::loaded_map::ptr loadedMap = m.load();
	
	float rectWidth = mapPreview.frame.size.width/30;
	float rectheight = mapPreview.frame.size.height/20;
	
	
	NSImage* previewImage = [[NSImage alloc] initWithSize:NSMakeSize(rectWidth*30, rectheight*20)];
	[previewImage setFlipped:YES];
	
	[previewImage lockFocus];
	

	
	for(int x = 0; x < 30; ++x) {
		for(int y = 0; y < 20; ++y) {
			const aw::terrain::ptr& terrain = loadedMap->m_terrain[x][y];
			const aw::unit::ptr& unit = loadedMap->m_unit[x][y];

			if(terrain) {
				switch(terrain->type()) {
					case aw::terrain::PLAIN:
					case aw::terrain::WRECKAGE:
						[[NSColor colorWithDeviceRed:204/255.0 green:240/255.0 blue:70/255.0 alpha:1.0] setFill]; 
						break;
					case aw::terrain::WATER:
					case aw::terrain::SEASIDE:
						[[NSColor colorWithDeviceRed:40/255.0 green:1750/255.0 blue:240/255.0 alpha:1.0] setFill]; 
						break;
					case aw::terrain::FOREST:
						[[NSColor colorWithDeviceRed:80/255.0 green:160/255.0 blue:160/255.0 alpha:1.0] setFill]; 
						break;
					case aw::terrain::REEF:
						[[NSColor colorWithDeviceRed:50/255.0 green:110/255.0 blue:250/255.0 alpha:1.0] setFill]; 
						break;
					case aw::terrain::MOUNTAIN:
						[[NSColor colorWithDeviceRed:255 green:230/255.0 blue:50/255.0 alpha:1.0] setFill]; 
						break;
					case aw::terrain::PIPE:
					case aw::terrain::SEGMENT_PIPE:
						[[NSColor colorWithDeviceRed:200/255.0 green:110/255.0 blue:130/255.0 alpha:1.0] setFill]; 
						break;
					case aw::terrain::BRIDGE:
						[[NSColor colorWithDeviceWhite:1/2.0 alpha:1.0] setFill]; 
						break;
					case aw::terrain::STREET:
						[[NSColor lightGrayColor] setFill]; 
						break;
					case aw::terrain::RIVER:
						[[NSColor colorWithDeviceRed:80/255.0 green:130/255.0 blue:255 alpha:1.0] setFill]; 
						break;
					case aw::terrain::BEACH:
						[[NSColor colorWithDeviceRed:255.0 green:240/255.0 blue:80/255.0 alpha:1.0] setFill]; 
						break;
						
					default:
						[[NSColor whiteColor] setFill];
				}
				
				
				if(terrain->is_building()) {
					
					if(terrain->type() != aw::terrain::HEADQUARTER) {
						switch(terrain->extra()) {
							case aw::terrain::RED:
								[[NSColor redColor] setFill]; 
								break;
							case aw::terrain::GREEN:
								[[NSColor greenColor] setFill]; 
								break;
							case aw::terrain::BLUE:
								[[NSColor blueColor] setFill]; 
								break;
							case aw::terrain::YELLOW:
								[[NSColor yellowColor] setFill]; 
								break;
							case aw::terrain::BLACK:
								[[NSColor darkGrayColor] setFill]; 
								break;
							case aw::terrain::WHITE:
								[[NSColor whiteColor] setFill]; 
								break;
						}
					} else {
						switch(terrain->extra()) {
							case aw::terrain::RED:
								[[NSColor colorWithDeviceRed:210/255.0 green:0.0 blue:0.0 alpha:1.0] setFill]; 
								break;
							case aw::terrain::GREEN:
								[[NSColor colorWithDeviceRed:0.0 green:210/255.0 blue:0.0 alpha:1.0] setFill]; 
								break;
							case aw::terrain::BLUE:
								[[NSColor colorWithDeviceRed:0.0 green:0.0 blue:210/255.0 alpha:1.0] setFill]; 
								break;
							case aw::terrain::YELLOW:
								[[NSColor colorWithDeviceRed:210/255.0 green:210/255.0 blue:0.0 alpha:1.0] setFill]; 
								break;
							case aw::terrain::BLACK:
								[[NSColor blackColor] setFill]; 
								break;
							case aw::terrain::WHITE:
								[[NSColor whiteColor] setFill]; 
								break;
						}
					}	
				}
				
				NSRectFill(NSMakeRect(x*rectWidth, y*rectheight, rectWidth, rectheight));
			}
			
			if(unit && ! unit->is_dummy()) {				
				switch(unit->color()) {
					case aw::unit::RED:
						[[NSColor redColor] setFill]; 
						break;
					case aw::unit::GREEN:
						[[NSColor greenColor] setFill]; 
						break;
					case aw::unit::BLUE:
						[[NSColor blueColor] setFill]; 
						break;
					case aw::unit::YELLOW:
						[[NSColor yellowColor] setFill]; 
						break;
					case aw::unit::BLACK:
						[[NSColor darkGrayColor] setFill]; 
						break;
					default:
						[[NSColor whiteColor] setFill]; 
						break;
				}
				
				NSRectFill(NSMakeRect(x*rectWidth, y*rectheight, rectWidth/1.8, rectheight/1.8));
			}
		}
	}
	
		
	
	[previewImage unlockFocus];
	
	//[mapPreview setImage:previewImage];
	[previewImage autorelease];
	return previewImage;

}

#pragma mark WindowController methods 

- (void)windowDidLoad {
	if(self.mapFile)
		[NSThread detachNewThreadSelector:@selector(createMapPreviewThreaded) toTarget:self withObject:nil];
}

#pragma mark Window Delegate Methods

- (void)windowWillClose:(NSNotification *)notification {
	[[NSApplication sharedApplication] stopModal];
}

@end
