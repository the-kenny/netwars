#import "BuyMenuController.h"

#import "Sprites.h"

#include "game/gui/paths.h"
#include "game/units/unit_loader.h"

@implementation BuyMenuController

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
			
			NSString* path = [NSString stringWithCString:aw::gui::get_path(loaded_unit.internal_name, player->get_unit_color()).c_str()];
			[dict setValue:[[Sprites sharedSprites] getSprite:path] forKey:@"image"];
			
			[unitArray addObject:dict];
		}
	}
	
	[unitArray sortUsingFunction:priceSort context:nil];
	
	[self showWindow:self];
		
	return self;
}

-(void)dealloc {
	[unitArray dealloc];
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


#pragma mark Sort Methods

NSInteger priceSort(NSDictionary* d1, NSDictionary* d2, void *context) {
	int price1 = [[d1 valueForKey:@"price"] intValue];
	int price2 = [[d2 valueForKey:@"price"] intValue];
	
	if (price1 < price2)
        return NSOrderedAscending;
    else if (price1 > price2)
		return NSOrderedDescending;
    else
        return NSOrderedSame;
}

NSInteger nameSort(NSDictionary* d1, NSDictionary* d2, void *context) {
	NSString* name1 = [d1 valueForKey:@"name"];
	NSString* name2 = [d2 valueForKey:@"name"];
	
	return [name1 compare:name2];
}

#pragma mark NSTableView Data Source Methods 

-(NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
	return unitArray.count;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex {
	return [[unitArray objectAtIndex:rowIndex] valueForKey:tableColumn.identifier];
}

#pragma mark NSTableView Delegate Methods

- (void)tableView:(NSTableView *)aTableView willDisplayCell:(id)aCell 
   forTableColumn:(NSTableColumn *)aTableColumn 
			  row:(NSInteger)rowIndex {
	
	if([[[unitArray objectAtIndex:rowIndex] valueForKey:@"price"] intValue] > player->get_funds())
		[aCell setEnabled:NO];
	else
		[aCell setEnabled:YES];
}

- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)rowIndex {
	if([[[unitArray objectAtIndex:rowIndex] valueForKey:@"price"] intValue] > player->get_funds()) {
		return NO;
	} else {
		returnValue = [[[unitArray objectAtIndex:rowIndex] valueForKey:@"internalName"] UTF8String];
		[buyButton setEnabled:YES];
		return YES;
	}
}

- (void)tableView:(NSTableView *)aTableView didClickTableColumn:(NSTableColumn *)aTableColumn {
	if([aTableColumn.identifier compare:@"price"] == NSOrderedSame) {
		[unitArray sortUsingFunction:priceSort context:nil];
		
		[aTableView setIndicatorImage:[NSImage imageNamed:@"NSAscendingSortIndicator"] inTableColumn:aTableColumn];
		[aTableView setIndicatorImage:nil inTableColumn:[aTableView tableColumnWithIdentifier:@"name"]];
	} else if([aTableColumn.identifier compare:@"name"] == NSOrderedSame) {
		[unitArray sortUsingFunction:nameSort context:nil];
		
		[aTableView setIndicatorImage:[NSImage imageNamed:@"NSAscendingSortIndicator"] inTableColumn:aTableColumn];
		[aTableView setIndicatorImage:nil inTableColumn:[aTableView tableColumnWithIdentifier:@"price"]];
	}
	
	[aTableView reloadData];
}

#pragma mark Window Delegate Methods

- (void)windowWillClose:(NSNotification *)notification {
	//Prevent the NSTableView from requesting data from a deallocated DataSource 
	[tableView setDelegate:nil];
	[tableView setDataSource:nil];
}

@end
