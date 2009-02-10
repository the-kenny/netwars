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
	transporterInfo = [[NSMutableDictionary alloc] init];;
	
	return self;
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[unitInfo release];
	
	[super dealloc];
}

-(void)showUnit:(const aw::unit::ptr&)u {
	
	if(u) {
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
		
		aw::transporter::ptr transporter = boost::dynamic_pointer_cast<aw::transporter>(u);
		if(transporter) {
			[transporterInfo setValue:[NSNumber numberWithInt:transporter->loaded_units_count()] forKey:@"usedSpace"];
			[transporterInfo setValue:[NSNumber numberWithInt:transporter->max_loaded_units()] forKey:@"totalSpace"];
			
			NSMutableArray* array = [NSMutableArray array];
			
			BOOST_FOREACH(const aw::unit::ptr &p, transporter->get_unit_list()) {
				NSString* path = [NSString stringWithCString:aw::gui::get_path(p->type(), p->color()).c_str()];
				[unitInfo setValue:[[Sprites sharedSprites] getSprite:path] forKey:@"image"];
				
				NSDictionary* dict = [NSDictionary dictionaryWithObjectsAndKeys:[[Sprites sharedSprites] getSprite:path], @"image",
									  [NSString stringWithCString:aw::unit_list().get_unit_info(p->get_name()).name.c_str()], @"realName", nil];
				[array addObject:dict];
				
			}
			
			[transporterInfo setValue:array forKey:@"loadedUnits"];
			
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
