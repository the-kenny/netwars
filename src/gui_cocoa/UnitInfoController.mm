#import "UnitInfoController.h"
#import "Sprites.h" 
#import "NotificationNames.h"
#import "CocoaUnit.h"

#include "game/gui/paths.h"
#include "game/units/unit_loader.h"

@implementation UnitInfoController

- (id)init {	
	self = [super initWithWindowNibName:@"UnitInfo"];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(showUnitNotificationCallback:)
												 name:unitClickedNotification
											   object:nil];
	
	return self;
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[super dealloc];
}

-(void)showUnit:(const aw::unit::ptr&)u {
	const std::string realName = aw::unit_list().get_unit_info(u->get_name()).name;
	[name setStringValue:[NSString stringWithCString:realName.c_str()]];
	
	[sprite setImage:[[Sprites sharedSprites] getSprite:[NSString stringWithCString:aw::gui::get_path(u->type(), u->color()).c_str()]]];
	[hp setIntValue:u->get_hp()];
	[fuel setIntValue:u->fuel()];
}

#pragma mark Notification Callbacks

-(void)showUnitNotificationCallback:(NSNotification*)notification {
	CocoaUnit* cocoaUnit = [[notification userInfo] objectForKey:@"unit"];
	
	[self showUnit:[cocoaUnit unit]];
}
@end
