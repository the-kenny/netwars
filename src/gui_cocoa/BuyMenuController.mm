#import "BuyMenuController.h"

#import "CocoaUnit.h"
#import "NotificationNames.h"
#import "Sprites.h"

#include "game/gui/paths.h"
#include "game/units/unit_loader.h"

@implementation BuyMenuController

#pragma mark Properties

@synthesize unitPrice;
@synthesize remFunds;

@dynamic playerFunds;

- (int)playerFunds {
	if(player)
		return player->get_funds();
	else
		return 0;
}

#pragma mark Standard Stuff

-(BuyMenuController*)initWithPlayer:(aw::player::ptr)p workshop:(aw::unit::workshops)ws {
	self = [super initWithWindowNibName:@"BuyMenu"];	
	
	player = p;
	workshop = ws;
	
	unitArray = [[NSMutableArray alloc] initWithCapacity:10];
	
	BOOST_FOREACH(const aw::unit_loader::value_type &pair, aw::unit_list()) {
		const aw::unit_loader::loaded_unit& loaded_unit = pair.second;
		
		if(!(loaded_unit.unit->is_dummy()) && loaded_unit.produced_in == workshop) {
			NSMutableDictionary* dict = [[NSMutableDictionary alloc] initWithCapacity:4];
			
			[dict setValue:[NSString stringWithCString:loaded_unit.internal_name.c_str()] forKey:@"internalName"];
			[dict setValue:[NSString stringWithCString:loaded_unit.name.c_str()] forKey:@"name"];
			[dict setValue:[NSNumber numberWithInt:loaded_unit.price] forKey:@"price"];
			[dict setValue:[NSNumber numberWithBool:(loaded_unit.price <= self.playerFunds)]  forKey:@"affordable"];
			[dict setValue:[CocoaUnit cocoaUnitWithUnit:loaded_unit.create_unit(p->get_unit_color())]  forKey:@"unit"];
			
			NSString* path = [NSString stringWithCString:aw::gui::get_path(loaded_unit.internal_name, player->get_unit_color()).c_str()];
			[dict setValue:[[Sprites sharedSprites] getSprite:path] forKey:@"image"];
			
			[unitArray addObject:dict];
			
			[dict release];
		}
	}
		
	[self showWindow:self];
		
	return self;
}

-(void)dealloc {
	[unitArray release];
	[super dealloc];
}

-(aw::unit::types)run {	
	[self showWindow:self];
	[[NSApplication sharedApplication] runModalForWindow:self.window];
	[self close];
	return returnValue;
}

-(IBAction)buyButtonClicked:(NSButton*)button {
	[[NSApplication sharedApplication] stopModal];
}

-(IBAction)cancelButtonClicked:(NSButton*)button {
	returnValue = aw::unit::TYPE_NONE;
	[[NSApplication sharedApplication] stopModal];
}

#pragma mark NSTableView Delegate Methods

- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)rowIndex {
	if([[[unitArray objectAtIndex:rowIndex] valueForKey:@"price"] intValue] <= player->get_funds())
		returnValue = [[[unitArray objectAtIndex:rowIndex] valueForKey:@"internalName"] UTF8String];
		
	self.unitPrice = [[[unitArray objectAtIndex:rowIndex] valueForKey:@"price"] intValue];
	self.remFunds = (self.playerFunds - self.unitPrice);
	
	//Trigger unitSelected-Notification
	NSDictionary* userInfo = [NSDictionary dictionaryWithObject:[[unitArray objectAtIndex:rowIndex] valueForKey:@"unit"] forKey:@"unit"];
	[[NSNotificationCenter defaultCenter] postNotificationName:unitClickedNotification
														object:self
													  userInfo:userInfo];
	
	return YES;
}

#pragma mark Window Delegate Methods

- (void)windowWillClose:(NSNotification *)notification {
	//Prevent the NSTableView from requesting data from a deallocated DataSource 
	[tableView setDelegate:nil];
	[tableView setDataSource:nil];
}

@end
