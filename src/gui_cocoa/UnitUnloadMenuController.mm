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

-(UnitUnloadMenuController*)init {
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

-(void)setUnits:(const std::list<aw::unit::ptr> &)units {
	int i = 0;
	BOOST_FOREACH(const aw::unit::ptr& p, units) {
		[self addUnit:p position:i];
		++i;
	}
}

- (void)addUnit:(const aw::unit::ptr&)unit position:(int)pos {
	NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 80, 20)];
	
	[button setTitle:[NSString stringWithCString:aw::unit_list().get_unit_info(unit->type()).name.c_str()]];
	
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];
	
	[button setImagePosition:NSImageLeft];
	
	NSString* image = [NSString stringWithCString:aw::gui::get_path(unit->type(), unit->color()).c_str()];
	[button setImage:[[Sprites sharedSprites] getSprite:image]];
	
	[button setTag:pos];
	[button setAction:@selector(menuItemClicked:)];
	[button setTarget:self];
	
	//Resize the window
	[self.window setFrame:NSMakeRect(self.window.frame.origin.x, self.window.frame.origin.x, 
									 self.window.frame.size.width, self.window.frame.size.height+24) display:YES];
	
	//Resize the vboxView
	[vboxView setFrame:NSMakeRect(vboxView.frame.origin.x, vboxView.frame.origin.x, vboxView.frame.size.width, vboxView.frame.size.height+24)];
	
	[vboxView addItem:button];
}

- (int)run:(NSPoint)pos {
	//Add the Cancel-Button
	NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 80, 20)];
	
	[button setTitle:@"Cancel"];
	
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];
	
	[button setTag:-1];
	[button setAction:@selector(menuItemClicked:)];
	[button setTarget:self];
	
	//Resize the window
	[self.window setFrame:NSMakeRect(self.window.frame.origin.x, self.window.frame.origin.x, 
									 self.window.frame.size.width, self.window.frame.size.height+24) display:YES];
	
	//Resize the vboxView
	[vboxView setFrame:NSMakeRect(vboxView.frame.origin.x, vboxView.frame.origin.x, vboxView.frame.size.width, vboxView.frame.size.height+24)];
	
	[vboxView addItem:button];
	
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
