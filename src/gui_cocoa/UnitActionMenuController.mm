#import "UnitActionMenuController.h"


#include <boost/foreach.hpp>

#include <iostream>

@implementation UnitActionMenuController

- (UnitActionMenuController*)initWithActions:(std::list<aw::units::actions>)actions {
	self = [super init];

	NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 80, 20*actions.size()+10) 
												   styleMask:NSTexturedBackgroundWindowMask  
													 backing:NSBackingStoreRetained 
													   defer:YES];
	self.window = window;
	
	vboxView = [[VBoxView alloc] initWithFrame:NSMakeRect(0, 0, 80, actions.size()*20)];
	[self.window.contentView addSubview:vboxView];

	BOOST_FOREACH(aw::units::actions a, actions) {
		NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 80, 20)];
		[button setButtonType:NSMomentaryLightButton];
		[button setTag:(NSInteger)a];
		
		[button setAction:@selector(menuItemClicked:)];
		[button setTarget:self];
		
		[vboxView addItem:button];
	}

	return self;
}

- (void)dealloc {
	[vboxView dealloc];
	[super dealloc];
}

- (aw::units::actions)run:(Coordinate*)c {
	[self.window setFrameOrigin:NSMakePoint(c.coord.x, (c.coord.y)-(self.window.frame.size.height))];
	
	[self showWindow:self];
	[[NSApplication sharedApplication] runModalForWindow:self.window];
	[self close];
	return returnAction;
}

#pragma mark "Callbacks"

-(void)menuItemClicked:(NSButton*)button {
	returnAction = (aw::units::actions)button.tag;
	[[NSApplication sharedApplication] stopModal];
}


@end
