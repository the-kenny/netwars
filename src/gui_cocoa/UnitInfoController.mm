#import "UnitInfoController.h"
#import "Sprites.h" 
#import "NotificationNames.h"
#import "CocoaUnit.h"

#include <boost/shared_ptr.hpp>

#include "game/gui/paths.h"
#include "game/units/unit_loader.h"
#include "game/units/unit_base.h"
#include "game/units/transporter.h"


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

-(NSDictionary*)getUnitInfo:(const aw::unit::ptr&)u {
	NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];

	NSString* path = [NSString stringWithCString:aw::gui::get_path(u->type(), u->color()).c_str()];
	[dict setValue:[[Sprites sharedSprites] getSprite:path] forKey:@"image"];

	[dict setValue:[NSNumber numberWithInt:u->max_hp()] forKey:@"maxHp"];
	[dict setValue:[NSNumber numberWithInt:u->hp()] forKey:@"hp"];

	[dict setValue:[NSNumber numberWithInt:u->max_fuel()] forKey:@"maxFuel"];
	[dict setValue:[NSNumber numberWithInt:u->fuel()] forKey:@"fuel"];

	[dict setValue:[NSNumber numberWithInt:u->moved()] forKey:@"hasMoved"];
	[dict setValue:[NSNumber numberWithInt:u->movement_range()] forKey:@"movementRange"];

	[dict setValue:[NSNumber numberWithBool:u->has_shot()] forKey:@"hasShot"];

	/*
	[dict setValue:[NSNumber numberWithInt:u->attack_range()] forKey:@"attackRange"];
	[dict setValue:[NSNumber numberWithInt:u->attack_distance()] forKey:@"attackDistance"];
	[dict setValue:[NSNumber numberWithInt:u->attack_range()+u->attack_distance()] forKey:@"realAttackRange"];
	 */
	
	[dict setValue:[NSNumber numberWithInt:u->explosion_range()] forKey:@"explosionRange"];
	[dict setValue:[NSNumber numberWithInt:u->explosion_damage()] forKey:@"explosionDamage"];

	[dict setValue:[NSString stringWithCString:u->type().c_str()] forKey:@"internalName"];
	[dict setValue:[NSString stringWithCString:aw::unit_list().get_unit_info(u->name()).name.c_str()] forKey:@"realName"];
	
	
	//We use an string because we can't do conditional formatting with bindings (as far as I know)
	
	if(u->attack_range() != -1) {
		if(u->attack_distance() == 0)
			[dict setValue:[NSString stringWithFormat:@"%i", u->attack_range()] forKey:@"rangeString"];
		else
			[dict setValue:[NSString stringWithFormat:@"%i to %i", u->attack_distance(), u->attack_range()+u->attack_distance()] forKey:@"rangeString"];
	}

	aw::transporter::ptr transporter = boost::dynamic_pointer_cast<aw::transporter>(u);
	[dict setValue:[NSNumber numberWithBool:(transporter != NULL)] forKey:@"isTransporter"];

	if(transporter != NULL) {
		[dict setValue:[NSNumber numberWithInt:transporter->loaded_units_count()] forKey:@"usedSpace"];
		[dict setValue:[NSNumber numberWithInt:transporter->max_loaded_units()] forKey:@"totalSpace"];

		NSMutableArray* array = [NSMutableArray array];

		BOOST_FOREACH(const aw::unit::ptr &p, transporter->get_unit_list()) {
			[array addObject:[self getUnitInfo:p]];
		}

		[dict setValue:array forKey:@"loadedUnits"];
	}

	return dict;
}

-(void)showUnit:(const aw::unit::ptr&)u {
	
	if(u) {
		[unitInfo setDictionary:[self getUnitInfo:u]];

		if([[unitInfo valueForKey:@"isTransporter"] boolValue]) {
			[drawer open];
		} else {
			[drawer close];
		}
	}
}

#pragma mark Notification Callbacks

-(void)showUnitNotificationCallback:(NSNotification*)notification {
	CocoaUnit* cocoaUnit = [[notification userInfo] objectForKey:@"unit"];
	
	[self showUnit:[cocoaUnit unit]];
}
@end
