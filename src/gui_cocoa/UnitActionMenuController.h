#import <Cocoa/Cocoa.h>
#import "VBoxView.h"
#import "Coordinate.h"

#include <list>

#include "game/units/actions.h"

@interface UnitActionMenuController : NSObject {
	VBoxView* vboxView;
	NSWindow* window;
	
	aw::units::actions returnAction;
}

//- (UnitActionMenuController*)initWithActions:(std::list<aw::units::actions>)actions;
- (UnitActionMenuController*)init;

- (void)addAction:(aw::units::actions)action;
- (aw::units::actions)run:(Coordinate*)c;

@end
