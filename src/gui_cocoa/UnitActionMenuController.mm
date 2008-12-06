#import "UnitActionMenuController.h"

#include "game/gui/unit_action_menu.h"

#include <boost/foreach.hpp>

#include <iostream>

@implementation UnitActionMenuController

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
	
	NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 80, 0)  //Size for actions.size buttons + the cancel-button
												   styleMask:NSBorderlessWindowMask  
													 backing:NSBackingStoreRetained 
													   defer:NO];
	self.window = window;
	
	vboxView = [[VBoxView alloc] initWithFrame:NSMakeRect(0, 0, 80, 0)];
	[vboxView setSpacing:0];
	[self.window.contentView addSubview:vboxView];
	
	return self;
}

- (void)dealloc {
	[vboxView dealloc];
	[super dealloc];
}

- (void)addAction:(aw::units::actions)action {
	NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 80, 20)];
	
	[button setTitle:[NSString stringWithCString:aw::gui::unit_action_menu::get_name(action).c_str()]];
	
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];
	
	[button setTag:(NSInteger)action];
	[button setAction:@selector(menuItemClicked:)];
	[button setTarget:self];
	
	//Resize the window
	[self.window setFrame:NSMakeRect(self.window.frame.origin.x, self.window.frame.origin.x, 
									 self.window.frame.size.width, self.window.frame.size.height+24) display:YES];
	
	//Resize the vboxView
	[vboxView setFrame:NSMakeRect(vboxView.frame.origin.x, vboxView.frame.origin.x, vboxView.frame.size.width, vboxView.frame.size.height+24)];
	
	[vboxView addItem:button];
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
