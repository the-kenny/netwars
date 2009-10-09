#import <Cocoa/Cocoa.h>

#include "game/units/unit_base.h"

@interface UnitInfoController : NSWindowController {
	aw::unit::ptr unit;
	NSMutableDictionary* unitInfo;
	
	IBOutlet NSDrawer* drawer;
}

-(NSDictionary*)getUnitInfo:(const aw::unit::ptr&)u;
-(void)showUnit:(const aw::unit::ptr&)u;

-(IBAction)toggleWindow:(id)sender;

@end
