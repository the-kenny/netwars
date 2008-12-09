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
