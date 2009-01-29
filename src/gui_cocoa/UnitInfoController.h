#import <Cocoa/Cocoa.h>

#include "game/units/unit_base.h"

@interface UnitInfoController : NSWindowController {
    IBOutlet id fuel;
    IBOutlet id hp;
    IBOutlet id name;
    IBOutlet id sprite;
}

-(void)showUnit:(aw::unit::ptr&)u;

@end
