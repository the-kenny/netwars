#import "UnitActionMenuController.h"

#include "game/gui/unit_action_menu.h"

#include <boost/foreach.hpp>

#include <iostream>

@implementation UnitActionMenuController

const unsigned int menuWidth = 60;
const unsigned int buttonHeight = 15;


/*
- (UnitActionMenuController*)initWithActions:(std::list<aw::units::actions>)actions {
	self = [super init];

	NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 80, 24+(24*actions.size()))  //Size for actions.size buttons + the cancel-button
												   styleMask:NSBorderlessWindowMask  
													 backing:NSBackingStoreRetained 
													   defer:NO];
	self.window = window;
	
	vboxView = [[VBoxView alloc] initWithFrame:NSMakeRect(0, 0, 80, actions.size()*20)];
	[vboxView setSpacing:0];
	[self.window.contentView addSubview:vboxView];
	
	BOOST_FOREACH(aw::units::actions a, actions) {
		[self addAction:a];
	}
	
	return self;
}
*/

- (UnitActionMenuController*)init {
	self = [super init];
	
	NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, menuWidth, 0)  //Size for actions.size buttons + the cancel-button
												   styleMask:NSBorderlessWindowMask  
													 backing:NSBackingStoreRetained 
													   defer:NO];
	self.window = window;
	
	vboxView = [[VBoxView alloc] initWithFrame:NSMakeRect(0, 0, menuWidth, 0)];
	[vboxView setSpacing:0];
	[self.window.contentView addSubview:vboxView];
	
	return self;
}

- (void)dealloc {
	[vboxView release];
	[super dealloc];
}

- (void)addAction:(aw::units::actions)action {
	NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, menuWidth, buttonHeight)];
	
	[button setTitle:[NSString stringWithCString:aw::gui::unit_action_menu::get_name(action).c_str()]];
	
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];
	[button setFont:[NSFont systemFontOfSize:[NSFont smallSystemFontSize]]];
	
	[button setTag:(NSInteger)action];
	[button setAction:@selector(menuItemClicked:)];
	[button setTarget:self];

	[vboxView addItem:button];
	[button release];
	
	//Resize the window
	[self.window setFrame:[vboxView frame] display:YES];
}

- (aw::units::actions)run:(Coordinate*)c {
	//Add the Cancel-Button
	[self addAction:aw::units::CANCEL];
	
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
