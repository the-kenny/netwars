#import "UnitInfoController.h"
#import "Sprites.h" 

#include "game/gui/paths.h"

@implementation UnitInfoController

-(void)showUnit:(aw::unit::ptr&)u {
	[name setString:[NSString stringWithCString:u->get_name().c_str()]];
	[sprite setImage:[[Sprites sharedSprites] getSprite:[NSString stringWithCString:aw::gui::get_path(u->type(), u->color()).c_str()]]];
	[hp setIntValue:u->get_hp()];
	[fuel setIntValue:u->fuel()];
}

@end
