#import <Cocoa/Cocoa.h>

#include "game/units/unit_base.h"

@interface UnitInfoController : NSWindowController {
	aw::unit::ptr unit;
	NSMutableDictionary* unitInfo;
}

-(void)showUnit:(const aw::unit::ptr&)u;

@end
