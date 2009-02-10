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
	
	unitInfo = [[NSMutableDictionary alloc] init];
	
	return self;
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[unitInfo release];
	
	[super dealloc];
}

-(void)showUnit:(const aw::unit::ptr&)u {
	NSString* path = [NSString stringWithCString:aw::gui::get_path(u->type(), u->color()).c_str()];
	[unitInfo setValue:[[Sprites sharedSprites] getSprite:path] forKey:@"image"];
	
	[unitInfo setValue:[NSNumber numberWithInt:u->max_hp()] forKey:@"maxHp"];
	[unitInfo setValue:[NSNumber numberWithInt:u->get_hp()] forKey:@"hp"];

	[unitInfo setValue:[NSNumber numberWithInt:u->max_fuel()] forKey:@"maxFuel"];
	[unitInfo setValue:[NSNumber numberWithInt:u->fuel()] forKey:@"fuel"];
	
	[unitInfo setValue:[NSNumber numberWithInt:u->moved()] forKey:@"hasMoved"];
	[unitInfo setValue:[NSNumber numberWithInt:u->movement_range()] forKey:@"movementRange"];

	[unitInfo setValue:[NSNumber numberWithBool:u->has_shot()] forKey:@"hasShot"];

	[unitInfo setValue:[NSNumber numberWithInt:u->get_attack_range()] forKey:@"attackRange"];
	[unitInfo setValue:[NSNumber numberWithInt:u->get_attack_distance()] forKey:@"attackDistance"];
	[unitInfo setValue:[NSNumber numberWithInt:u->get_attack_range()+u->get_attack_distance()] forKey:@"realAttackRange"];

	[unitInfo setValue:[NSNumber numberWithInt:u->get_explosion_range()] forKey:@"explosionRange"];
	[unitInfo setValue:[NSNumber numberWithInt:u->get_explosion_damage()] forKey:@"explosionDamage"];

	[unitInfo setValue:[NSString stringWithCString:u->type().c_str()] forKey:@"internalName"];
	[unitInfo setValue:[NSString stringWithCString:aw::unit_list().get_unit_info(u->get_name()).name.c_str()] forKey:@"realName"];
}

#pragma mark Notification Callbacks

-(void)showUnitNotificationCallback:(NSNotification*)notification {
	CocoaUnit* cocoaUnit = [[notification userInfo] objectForKey:@"unit"];
	
	[self showUnit:[cocoaUnit unit]];
}
@end
