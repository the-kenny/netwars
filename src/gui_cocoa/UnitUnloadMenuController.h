//
//  UnitUnloadMenu.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 06.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "VBoxView.h"

#include "game/units/unit_base.h"

#include <list>

@interface UnitUnloadMenuController : NSWindowController {
	VBoxView* vboxView;
	
	int returnPosition;
}

-(UnitUnloadMenuController*)init;
-(void)setUnits:(const std::list<aw::unit::ptr> &)units;
- (void)addUnit:(const aw::unit::ptr&)unit position:(int)pos;
-(int)run:(NSPoint)pos;

@end
