//
//  UnitUnloadMenu.mm
//  gui_cocoa
//
//  Created by Moritz Ulrich on 06.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "UnitUnloadMenuController.h"
#import "Sprites.h"

#include "game/units/unit_loader.h"
#include "game/gui/paths.h"


@implementation UnitUnloadMenuController

const unsigned int menuWidth = 80;
const unsigned int buttonHeight = 16;

-(UnitUnloadMenuController*)init {
	self = [super init];
	
	NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, menuWidth, 0)  //Size for actions.size buttons + the cancel-button
												   styleMask:NSBorderlessWindowMask  
													 backing:NSBackingStoreRetained 
													   defer:NO];
	self.window = window;
	
	[window release];
	
	vboxView = [[VBoxView alloc] initWithFrame:NSMakeRect(0, 0, menuWidth, 0)];
	[vboxView setSpacing:0];
	[self.window.contentView addSubview:vboxView];
	
	return self;
}

-(void)dealloc {
	[vboxView release];
	[super dealloc];
}

-(void)setUnits:(const std::list<aw::unit::ptr> &)units {
	int i = 0;
	BOOST_FOREACH(const aw::unit::ptr& p, units) {
		[self addUnit:p position:i];
		++i;
	}
}

- (void)addUnit:(const aw::unit::ptr&)unit position:(int)pos {
	NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, menuWidth, buttonHeight)];
	
	[button setTitle:[NSString stringWithCString:aw::unit_list().get_unit_info(unit->type()).name.c_str()]];
	
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];
	[button setFont:[NSFont systemFontOfSize:[NSFont smallSystemFontSize]]];
	
	[button setImagePosition:NSImageLeft];
	
	NSString* image = [NSString stringWithCString:aw::gui::get_path(unit->type(), unit->color()).c_str()];
	[button setImage:[[Sprites sharedSprites] getSprite:image]];
	
	[button setTag:pos];
	[button setAction:@selector(menuItemClicked:)];
	[button setTarget:self];

	[vboxView addItem:button];
	[button release];
	
	//Resize the window
	[self.window setFrame:[vboxView frame] display:YES];	
}

- (int)run:(NSPoint)pos {
	//Add the Cancel-Button
	NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, menuWidth, buttonHeight)];
	
	[button setTitle:@"Cancel"];
	
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];
	[button setFont:[NSFont systemFontOfSize:[NSFont smallSystemFontSize]]];
	
	[button setTag:-1];
	[button setAction:@selector(menuItemClicked:)];
	[button setTarget:self];

	[vboxView addItem:button];
	[button release];
	
	//Resize the window
	[self.window setFrame:[vboxView frame] display:YES];	
	
	//Move the window to the mouse
	[self.window setFrameOrigin:NSMakePoint(pos.x, (pos.y)-(self.window.frame.size.height))];
	
	[self showWindow:self];
	[[NSApplication sharedApplication] runModalForWindow:self.window];
	[self close];
	return returnPosition;
}

#pragma mark "Callbacks"

-(void)menuItemClicked:(NSButton*)button {
	returnPosition = (int)button.tag;
	[[NSApplication sharedApplication] stopModal];
}

@end
