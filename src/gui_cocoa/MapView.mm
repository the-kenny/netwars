#import "MapView.h"
#import "Coordinate.h"
#import "Sprites.h"

#import <QuartzCore/QuartzCore.h>

#include "game/gui/paths.h"
#include "game/terrain.h"
#include "game/gui/map_widget.h"
#include "game/gui/drawing.h"
#include "game/config.h"

#include <utility>

using namespace aw;
using namespace aw::gui;

#include <iostream>

NSString* mouseMovedNotification = @"mouseMoveOnMap";
NSString* leftMouseClickNotification = @"mouseClickOnMap";
NSString* rightMouseClickNotification = @"rightMouseClickOnMap";

@implementation MapView

@dynamic scene;

- (aw::scene::ptr)scene {
	return currentScene;
}

- (void)setScene:(aw::scene::ptr)scene {
	//[unitMovements release];
	[self processScene:scene];
	
	currentScene = scene;
	/*
	 
	 for(int x = 0; x < 30; x++) {
	 for(int y = 0; y < 20; y++) {
	 aw::unit::ptr unit = scene->get_unit(x, y);
	 
	 if(unit != NULL && !unit->is_dummy()) {
	 [self addUnitForDrawing:unit at:[self toViewCoordinates:NSMakePoint(x, y) 
	 rect:NSMakeSize(16, 16)]];
	 }
	 }
	 }
	 */
}

@synthesize isEnabled;

- (void)awakeFromNib {
	NSLog(@"MapView.awakeFromNib");
	trackingArea = [[NSTrackingArea alloc] initWithRect:NSMakeRect(0.0, 0.0, 480.0, 320.0) 
												options:NSTrackingMouseMoved|NSTrackingActiveInActiveApp
												  owner:self userInfo:nil];
	[self addTrackingArea:trackingArea];
	
	self.isEnabled = false;
	
	background = [[NSImage alloc] initByReferencingFile:[[NSBundle mainBundle] pathForResource:@"background" ofType:@"png" inDirectory:@"data"]];
	
	maskImage = [[NSImage alloc] initWithSize:NSMakeSize(64, 64)];
	[maskImage lockFocus];
	[NSGraphicsContext saveGraphicsState];
	
	[[NSColor colorWithDeviceRed:0.3 green:0.3 blue:0.3 alpha:1.0] setFill];
	NSRectFill(NSMakeRect(0.0, 0.0, maskImage.size.width, maskImage.size.height));
	
	[NSGraphicsContext restoreGraphicsState];
	[maskImage unlockFocus];
	
	managedUnits = [[NSMutableSet alloc] initWithCapacity:10];
}

-(void)dealloc {
	[background release];
	[maskImage release];
	[trackingArea release]; 
	[super dealloc];
}

//NOTE: There are problems with the CALayer drawing if the view is flipped
- (BOOL)isFlipped {
	return NO;
}

#pragma mark Unit-Animation Helper Methods

- (AnimatableUnit*)getAnimatableUnit:(aw::unit::ptr)u {
	std::map<aw::unit::ptr, AnimatableUnit*>::iterator it = unitMap.find(u);
	if(it != unitMap.end())
		return [it->second autorelease];
	else
		return nil;
}

- (void)processScene:(aw::scene::ptr&)newScene {
	aw::scene::ptr oldScene = [self scene];
	
	NSMutableArray* unitActs = [NSMutableArray arrayWithCapacity:1]; //There isn't a big chance for a scene with more than one change...
	
	if(newScene && oldScene) {
		std::map<aw::unit::ptr, aw::coord> oldUnits;
		std::map<aw::unit::ptr, aw::coord> newUnits;
		typedef std::map<aw::unit::ptr, aw::coord>::iterator iterator;
		
		//Cycle through the old scene and collect 
		for(int x = 0; x < 30; ++x) {
			for(int y = 0; y < 20; ++y) {
				const unit::ptr &oldUnit = oldScene->get_unit(x, y);
				const unit::ptr &newUnit = newScene->get_unit(x, y);
				
				if(oldUnit != NULL && !oldUnit->is_dummy()) {
					oldUnits.insert(std::make_pair(oldUnit, coord(x, y)));
				}
				
				if(newUnit != NULL && !newUnit->is_dummy()) {
					newUnits.insert(std::make_pair(newUnit, coord(x, y)));
				}
			}
		}
		
		typedef std::pair<aw::unit::ptr, aw::coord> pair;
		BOOST_FOREACH(pair newPair, newUnits) {
			//oldUnits.insert(std::make_pair(newUnit, coord(x, y)));
			
			iterator it = oldUnits.find(newPair.first);
			
			//Cycle through the old units and search for the new, then add it to the move-list
			if(it != oldUnits.end()) {
				//Add the unit to the movement-array when the position has changed
				if(newPair.second != it->second) {
					NSLog(@"Unit moved from %i|%i to %i|%i", it->second.x, it->second.y, newPair.second.x, newPair.second.y);
					[unitActs addObject:[NSDictionary dictionaryWithObjectsAndKeys:
										 @"moved", @"action",
										 [Coordinate coordinateWithCoordinates:it->second.x y:it->second.y], @"from",
										 [Coordinate coordinateWithCoordinates:newPair.second.x y:newPair.second.y], @"to", 
										 [self getAnimatableUnit:it->first], @"animatableUnit", nil]]; 
				}
			} else {
				//Unit isn't in the old scene - it is new.
				[self addUnitForDrawing:newPair.first at:[self toViewCoordinates:NSMakePoint(newPair.second.x, newPair.second.y) 
																			rect:NSMakeSize(16, 16)]];
				
				[unitActs addObject:[NSDictionary dictionaryWithObjectsAndKeys:
									 @"added", @"action",
									 [Coordinate coordinateWithCoordinates:it->second.x y:it->second.y], @"position",
									 [self getAnimatableUnit:newPair.first], @"animatableUnit", nil]]; 
			}
		}
		
		BOOST_FOREACH(pair oldPair, oldUnits) {
			iterator it = newUnits.find(oldPair.first);
		
			//Unit isn't in the new scene - it is gone.
			if(it == newUnits.end()) {
				[self removeUnitFromDrawing:oldPair.first];
				
				 /*
				[unitActs addObject:[NSDictionary dictionaryWithObjectsAndKeys:
									 @"removed", @"action",
									 [Coordinate coordinateWithCoordinates:it->second.x y:it->second.y], @"position",
									 [self getAnimatableUnit:oldPair.first], @"animatableUnit", nil]]; 
				 */
			}
		}
	} else if(newScene) {
		//Cycle through the old scene and collect 
		for(int x = 0; x < 30; ++x) {
			for(int y = 0; y < 20; ++y) {
				const unit::ptr &unit = newScene->get_unit(x, y);
				
				if(unit != NULL && !unit->is_dummy()) {
					[self addUnitForDrawing:unit at:[self toViewCoordinates:NSMakePoint(x, y) 
																	   rect:NSMakeSize(16, 16)]];
				}
			}
		}
	}
	
	[unitActions release];
	unitActions = unitActs;
	[unitActions retain];
}

- (void)addUnitForDrawing:(aw::unit::ptr)u at:(NSPoint)at {
	if([self getAnimatableUnit:u])
		return;
	
	AnimatableUnit* au = [[AnimatableUnit alloc] init];
	au.unit = u;
	
	CALayer* layer = [CALayer layer];
	[layer setAnchorPoint:CGPointMake(.0, .0)];
	NSSize size = NSMakeSize(16, 16);
	CGRect rect;
	rect.origin = *(CGPoint*)&at;
	rect.size = *(CGSize*)&size;
	[layer setFrame:rect];
	[[self layer] addSublayer:layer];
	
	[layer retain];
	au.layer = layer;
	
	[au draw];
	
	[managedUnits addObject:au];
	unitMap.insert(std::make_pair(u, au));
	[au retain];
}

- (void)removeUnitFromDrawing:(aw::unit::ptr)u {
	AnimatableUnit* au = [self getAnimatableUnit:u];
	[managedUnits removeObject:au];
	unitMap.erase(u);
	
	[[au layer] removeFromSuperlayer];
	[au release];
};

#pragma mark "Event Handling"

- (void)mouseDown:(NSEvent *)theEvent {
	if(isEnabled) {
		NSPoint point = [self toGameCoordinates:[self convertPoint:[theEvent locationInWindow] fromView:nil]];

		[[NSNotificationCenter defaultCenter] 
		 postNotificationName:leftMouseClickNotification
		 object:self
		 userInfo: [NSDictionary dictionaryWithObject:[Coordinate coordinateWithPoint:point] forKey:@"Position"]];
	}
}

- (void)rightMouseDown:(NSEvent *)theEvent {
	if(isEnabled) {
		NSPoint point = [self toGameCoordinates:[self convertPoint:[theEvent locationInWindow] fromView:nil]];

		[[NSNotificationCenter defaultCenter] 
		 postNotificationName:rightMouseClickNotification
		 object:self
		 userInfo: [NSDictionary dictionaryWithObject:[Coordinate coordinateWithPoint:point] forKey:@"Position"]];
	}
}

- (void)mouseMoved:(NSEvent *)theEvent {	
	if(isEnabled) {
		NSPoint actualPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
		
		static int old_x;               
		static int old_y; 

		NSPoint p = [self toGameCoordinates:actualPoint];
		
		int x = static_cast<int>(p.x);
		int y = static_cast<int>(p.y);
		
		//y = abs(y-19);
		
		if((x != old_x || y != old_y) && (x < 30 && y < 20))
		{
			NSMutableDictionary* userData = [[NSMutableDictionary alloc] initWithCapacity:3];
			[userData setValue:[Coordinate coordinateWithCoordinates:x y:y] forKey:@"Position"];
			[userData setValue:[Coordinate coordinateWithPoint:actualPoint] forKey:@"PositionInPixels"];
			//[userData insertValue:[theEvent mouseLocation] inPropertyWithKey:@"GlobalPosition"];
			
			[[NSNotificationCenter defaultCenter] 
			 postNotificationName:mouseMovedNotification
			 object:self
			 userInfo:userData];
			
			[userData release];
			
			old_x = x;
			old_y = y;
		}
	}
}

#pragma mark "Drawing"

- (NSPoint)toViewCoordinates:(NSPoint)pos rect:(NSSize)size {
	//return NSMakePoint(pos.x*16+(16-size.width), pos.y*16+(16-size.height));
	
	NSSize viewSize = self.bounds.size;
	
	int unflippedX = pos.x*16;
	int unflippedY = pos.y*16;
	

	if(![self isFlipped])
		return NSMakePoint(unflippedX, viewSize.height-unflippedY-16);
	else
		return NSMakePoint(unflippedX, unflippedY+(16-size.height));
}

- (NSPoint)toGameCoordinates:(NSPoint)pos {
	int x = (pos.x/16);
	int y = ([self isFlipped]) ? (pos.y/16) : 20-(pos.y/16);
		
	return NSMakePoint(x, y);
}

- (void)draw:(NSString*)path at:(NSPoint)pos {
	if(path.length != 0) {
		NSImage* sprite = [[Sprites sharedSprites] getSprite:path];
		
		if([self isFlipped])
			[sprite setFlipped:YES]; //We have to flip the image because the view is flipped
		
		[sprite drawAtPoint:[self toViewCoordinates:pos rect:sprite.size] fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
		[sprite setFlipped:NO];
	}
}

- (void)drawRect:(NSRect)rect { 
	//NSLog(@"drawRect");
	[super drawRect:rect];
	
	//Draw the terrain
	[self drawTerrain];
	
	if(currentScene) {
		//CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		
		NSEnumerator* e = [unitActions objectEnumerator];
		NSDictionary* actions = nil;
		while(actions = [e nextObject]) {
			if([[actions valueForKey:@"actions"] compare:@"moved"] == NSOrderedSame) {
				//Coordinate* from = [actions valueForKey:@"from"];
				Coordinate* to = [actions valueForKey:@"to"];
				
				aw::unit::ptr u = currentScene->get_unit(to.coord.x, to.coord.y);
				
				AnimatableUnit* au = [self getAnimatableUnit:u];
				//[au setPosition:[self toViewCoordinates:to.point rect:NSMakeSize(16.0, 16.0)]];
				[au moveTo:[self toViewCoordinates:to.point rect:NSMakeSize(16.0, 16.0)]];
			}
		}
		
		[managedUnits makeObjectsPerformSelector:@selector(draw)];
		
		for(int x = 0; x < 30; x++)
		{   
			for(int y = 0; y < 20; y++)
			{   
				static const NSString* range = [[NSString stringWithCString:(aw::config().get<std::string>("/config/dirs/pixmaps") + "/misc/range.png").c_str()] retain];
				static const NSString* path = [[NSString stringWithCString:(aw::config().get<std::string>("/config/dirs/pixmaps") + "/misc/path.png").c_str()] retain];
				
				if(currentScene->highlighted(x, y)) 
					[self draw:range at:NSMakePoint(x, y)];
				
				if(currentScene->path(x, y)) 
					[self draw:path at:NSMakePoint(x, y)];
			}   
		}
		 
	}
		
	
	if(!isEnabled) {
		static NSColor* color = [NSColor colorWithDeviceRed:0.6 green:0.6 blue:0.6 alpha:0.5]; 
		[color setFill];
		//TODO: Magic numbers are bad!
		NSRectFillUsingOperation(NSMakeRect(0, 0, 480, 320), NSCompositeSourceOver);
	}
} 

- (void)drawTerrain {
	[background drawAtPoint:NSMakePoint(0, 0) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
	
	if(currentScene) {
		for(int x = 0; x < 30; x++)
		{
			for(int y = 0; y < 20; y++)
			{
				[self draw:[NSString stringWithCString:gui::get_sprite_for(aw::coord(x, y), currentScene).c_str()] at:NSMakePoint(x, y)];
				 
				/*
				const terrain::types type = currentScene->get_terrain(x, y)->type();
				const terrain::extras extra = currentScene->get_terrain(x, y)->extra();
				
				if(type == terrain::WATER)
				{
					if(y > 0 && x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y-1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::UL, terrain::CORNER).c_str()] at:NSMakePoint(x, y)]; //Ecke oben Links
					if(y < 19 && x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y+1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::LD, terrain::CORNER).c_str()] at:NSMakePoint(x, y)]; //Ecke unten links
					if(y > 0 && x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y-1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::UR, terrain::CORNER).c_str()] at:NSMakePoint(x, y)]; //Ecke oben rechts
					if(y < 19 && x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y+1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::DR, terrain::CORNER).c_str()] at:NSMakePoint(x, y)]; //Ecke unten rechts
					
					if(x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::L).c_str()] at:NSMakePoint(x, y)]; //Küste links
					if(x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::R).c_str()] at:NSMakePoint(x, y)]; //Küste rechts
					if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::U).c_str()] at:NSMakePoint(x, y)]; //Küste oben
					if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::D).c_str()] at:NSMakePoint(x, y)]; //Küste unten
					
					if(x > 0 && y < 19 && y > 0 && currentScene->get_terrain(x-1, y)->type() == terrain::RIVER &&
					   (currentScene->get_terrain(x-1, y+1)->type() != terrain::RIVER && currentScene->get_terrain(x-1, y-1)->type() != terrain::RIVER))
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::L, terrain::MOUTH).c_str()] at:NSMakePoint(x, y)]; //Flussmündung links
					
					if(x < 29 && y < 19 && y > 0 && currentScene->get_terrain(x+1, y)->type() == terrain::RIVER &&
					   (currentScene->get_terrain(x+1, y-1)->type() != terrain::RIVER && currentScene->get_terrain(x+1, y+1)->type() != terrain::RIVER))
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::R, terrain::MOUTH).c_str()] at:NSMakePoint(x, y)]; //Flussmündung rechts
					
					if(y > 0 && x < 29 && x > 0 && currentScene->get_terrain(x, y-1) ->type()== terrain::RIVER &&
					   (currentScene->get_terrain(x-1, y-1)->type() != terrain::RIVER && currentScene->get_terrain(x+1, y-1)->type() != terrain::RIVER))
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::U, terrain::MOUTH).c_str()] at:NSMakePoint(x, y)]; //Flussmündung oben
					
					if(y < 19 && x < 29 && x > 0 && currentScene->get_terrain(x, y+1)->type() == terrain::RIVER &&
					   (currentScene->get_terrain(x-1, y+1)->type() != terrain::RIVER && currentScene->get_terrain(x+1, y+1)->type() != terrain::RIVER))
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::D, terrain::MOUTH).c_str()] at:NSMakePoint(x, y)]; //Flussmündung unten
					
					if(x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y)->type()))
					{
						if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::UL).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (terrain::UL)
						
						if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::LD).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (LD)
					}
					
					if(x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
					{
						if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::UR).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (UR)
						else if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::DR).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (DR)
					}
					
					if(x > 0 && x < 29 && terrain::is_ground(currentScene->get_terrain(x-1, y)->type()) && terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
					{
						if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::ULR).c_str()] at:NSMakePoint(x, y)]; //Bucht nach unten offen
						else if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::LDR).c_str()] at:NSMakePoint(x, y)]; //Bucht nach oben offen
					}
					
					if(y > 0 && y < 19 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()) && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
					{
						if(x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::ULD).c_str()] at:NSMakePoint(x, y)]; //Bucht nach rechts offen
						else if(x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::UDR).c_str()] at:NSMakePoint(x, y)]; //Bucht nach links offen
					}
					
					if(y > 0 && y < 19 && x > 0 && x < 29)
					{
						if(terrain::is_ground(currentScene->get_terrain(x, y-1)->type()) && //Oben
						   terrain::is_ground(currentScene->get_terrain(x, y+1)->type()) && //Unten
						   terrain::is_ground(currentScene->get_terrain(x+1, y)->type()) && //Rechts
						   terrain::is_ground(currentScene->get_terrain(x-1, y)->type())) //Links
							[self draw:[NSString stringWithCString:get_path(terrain::SEASIDE, terrain::ULDR).c_str()] at:NSMakePoint(x, y)]; //See (oder was das sein soll)
					}
				}
				else if(type == terrain::RIVER)
				{
					if(y > 0 && y < 19 && x > 0 && x < 29 &&
					   is_connectable(terrain::RIVER, currentScene->get_terrain(x, y-1)->type()) && //Oben
					   is_connectable(terrain::RIVER, currentScene->get_terrain(x, y+1)->type()) && //Unten
					   is_connectable(terrain::RIVER, currentScene->get_terrain(x+1, y)->type()) && //Rechts
					   is_connectable(terrain::RIVER, currentScene->get_terrain(x-1, y)->type())) //Links)
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::ULDR).c_str()] at:NSMakePoint(x, y)]; //Flusskreuzung (ULDR)
					}
					else if(y > 0 && y < 19 && x > 0 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::RIVER, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::ULD).c_str()] at:NSMakePoint(x, y)]; //Fluss mündet in anderen Fluss (terrain::ULD)
					}
					else if(y > 0 && y < 19 && x < 29 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::RIVER, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::UDR).c_str()] at:NSMakePoint(x, y)]; //Fluss mündet in anderen Fluss (UDR)
					}
					else if(y < 19 && x > 0 && x < 29 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::RIVER, currentScene->get_terrain(x-1, y)->type()) &&//Links
							is_connectable(terrain::RIVER, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::LDR).c_str()] at:NSMakePoint(x, y)]; //Fluss mündet in anderen Fluss (LDR)
					}
					else if(y > 0 && x > 0 && x < 29 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::RIVER, currentScene->get_terrain(x-1, y)->type()) &&//Links
							is_connectable(terrain::RIVER, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::ULR).c_str()] at:NSMakePoint(x, y)]; //Fluss mündet in anderen Fluss (ULR)
					}
					else if(y > 0 && x > 0 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::RIVER, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::UL).c_str()] at:NSMakePoint(x, y)]; //Kurve (terrain::UL)
					}
					else if(y < 19 && x > 0 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::RIVER, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::LD).c_str()] at:NSMakePoint(x, y)]; //Kurve (LD)
					}
					else if(y > 0 && x < 29 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::RIVER, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::UR).c_str()] at:NSMakePoint(x, y)]; //Kurve (UR)
					}
					else if(y < 19 && x < 29 &&
							is_connectable(terrain::RIVER, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::RIVER, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::DR).c_str()] at:NSMakePoint(x, y)]; //Kurve (DR)
					}
					else if((y > 0 && is_connectable(terrain::RIVER, currentScene->get_terrain(x, y-1)->type())) || //Oben
							(y < 19 && is_connectable(terrain::RIVER, currentScene->get_terrain(x, y+1)->type())))
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::UD).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
					else if((x > 0 && is_connectable(terrain::RIVER, currentScene->get_terrain(x-1, y)->type())) ||
							(x < 29 && is_connectable(terrain::RIVER, currentScene->get_terrain(x+1, y)->type())))
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
					else if((y > 0 && currentScene->get_terrain(x, y-1)->type() == terrain::WATER) ||
							(y < 19 && currentScene->get_terrain(x, y+1)->type() == terrain::WATER)) //Um einzelne Flusstücke mit Flussmündungen zu verbinden
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::UD).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
					else if((x > 0 && currentScene->get_terrain(x-1, y)->type() == terrain::WATER) ||
							(x < 29 && currentScene->get_terrain(x+1, y)->type() == terrain::WATER)) //Um einzelne Flusstücke mit Flussmündungen zu verbinden
					{
						[self draw:[NSString stringWithCString:get_path(terrain::RIVER, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
				}
				else if(type == terrain::STREET)
				{
					if(y > 0 && y < 19 && x > 0 && x < 29 &&
					   is_connectable(terrain::STREET, currentScene->get_terrain(x, y-1)->type()) && //Oben
					   is_connectable(terrain::STREET, currentScene->get_terrain(x, y+1)->type()) && //Unten
					   is_connectable(terrain::STREET, currentScene->get_terrain(x+1, y)->type()) && //Rechts
					   is_connectable(terrain::STREET, currentScene->get_terrain(x-1, y)->type())) //Links)
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::ULDR).c_str()] at:NSMakePoint(x, y)]; //Kreuzung (ULDR)
					}
					else if(y > 0 && y < 19 && x > 0 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::STREET, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::ULD).c_str()] at:NSMakePoint(x, y)]; //Strasse trifft auf querstrasse (terrain::ULD)
					}
					else if(y > 0 && y < 19 && x < 29 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::STREET, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::UDR).c_str()] at:NSMakePoint(x, y)]; //Strasse trifft auf querstrasse (UDR)
					}
					else if(y < 19 && x > 0 && x < 29 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::STREET, currentScene->get_terrain(x-1, y)->type()) &&//Links
							is_connectable(terrain::STREET, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::LDR).c_str()] at:NSMakePoint(x, y)]; //Strasse trifft auf querstrasse (LDR)
					}
					else if(y > 0 && x > 0 && x < 29 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::STREET, currentScene->get_terrain(x-1, y)->type()) &&//Links
							is_connectable(terrain::STREET, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::ULR).c_str()] at:NSMakePoint(x, y)]; //Strasse trifft auf querstrasse (ULR)
					}
					else if(y > 0 && x > 0 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::STREET, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::UL).c_str()] at:NSMakePoint(x, y)]; //Kurve (terrain::UL)
					}
					else if(y < 19 && x > 0 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::STREET, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::LD).c_str()] at:NSMakePoint(x, y)]; //Kurve (LD)
					}
					else if(y > 0 && x < 29 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::STREET, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::UR).c_str()] at:NSMakePoint(x, y)]; //Kurve (UR)
					}
					else if(y < 19 && x < 29 &&
							is_connectable(terrain::STREET, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::STREET, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::DR).c_str()] at:NSMakePoint(x, y)]; //Kurve (DR)
					}
					else if((y > 0 && is_connectable(terrain::STREET, currentScene->get_terrain(x, y-1)->type())) || //Oben
							(y < 19 && is_connectable(terrain::STREET, currentScene->get_terrain(x, y+1)->type()))) //unten
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::UD).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
					else if((x > 0 && is_connectable(terrain::STREET, currentScene->get_terrain(x-1, y)->type())) || //Links
							(x < 29 && is_connectable(terrain::STREET, currentScene->get_terrain(x+1, y)->type()))) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
					else
					{
						[self draw:[NSString stringWithCString:get_path(terrain::STREET, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Gerade (LR)
					}
				}
				else if(type == terrain::BRIDGE)
				{
					if((y > 0 && is_connectable(terrain::BRIDGE, currentScene->get_terrain(x, y-1)->type())) || //Oben
					   (y < 19 && is_connectable(terrain::BRIDGE, currentScene->get_terrain(x, y+1)->type()))) //unten
					{
						[self draw:[NSString stringWithCString:get_path(terrain::BRIDGE, terrain::UD).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
					else if((x > 0 && is_connectable(terrain::BRIDGE, currentScene->get_terrain(x-1, y)->type())) || //Links
							(x < 29 && is_connectable(terrain::BRIDGE, currentScene->get_terrain(x+1, y)->type()))) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::BRIDGE, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Gerade (LR)
					}
				}
				else if(type == terrain::PIPE)
				{
					if(y > 0 && x > 0 &&
					   is_connectable(terrain::PIPE, currentScene->get_terrain(x, y-1)->type()) && //Oben
					   is_connectable(terrain::PIPE, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::UL).c_str()] at:NSMakePoint(x, y)]; //Kurve (terrain::UL)
					}
					else if(y < 19 && x > 0 &&
							is_connectable(terrain::PIPE, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::PIPE, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::LD).c_str()] at:NSMakePoint(x, y)]; //Kurve (LD)
					}
					else if(y > 0 && x < 29 &&
							is_connectable(terrain::PIPE, currentScene->get_terrain(x, y-1)->type()) && //Oben
							is_connectable(terrain::PIPE, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::UR).c_str()] at:NSMakePoint(x, y)]; //Kurve (UR)
					}
					else if(y < 19 && x < 29 &&
							is_connectable(terrain::PIPE, currentScene->get_terrain(x, y+1)->type()) && //Unten
							is_connectable(terrain::PIPE, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::DR).c_str()] at:NSMakePoint(x, y)]; //Kurve (DR)
					}
					else if(x > 0 && is_connectable(terrain::PIPE, currentScene->get_terrain(x-1, y)->type()) && //Links
							x < 29 && is_connectable(terrain::PIPE, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Gerade (LR)
					}
					else if(y > 0 && is_connectable(terrain::PIPE, currentScene->get_terrain(x, y-1)->type()) && //Oben
							y < 19 && is_connectable(terrain::PIPE, currentScene->get_terrain(x, y+1)->type())) //unten
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::UD).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
					else if(y > 0 && is_connectable(terrain::PIPE, currentScene->get_terrain(x, y-1)->type())) //Oben
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::U).c_str()] at:NSMakePoint(x, y)]; //Endstück (terrain::U)
					}
					else if(y < 19 && is_connectable(terrain::PIPE, currentScene->get_terrain(x, y+1)->type())) //Unten
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::D).c_str()] at:NSMakePoint(x, y)]; //Endstück (terrain::D)
					}
					else if(x > 0 && is_connectable(terrain::PIPE, currentScene->get_terrain(x-1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::L).c_str()] at:NSMakePoint(x, y)]; //Endstück (terrain::L)
					}
					else if(x < 29 && is_connectable(terrain::PIPE, currentScene->get_terrain(x+1, y)->type())) //Links
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::R).c_str()] at:NSMakePoint(x, y)]; //Endstück (terrain::R)
					}
					else
					{
						[self draw:[NSString stringWithCString:get_path(terrain::PIPE, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Wenn keine Verbindungen möglich sind (LR)
					}
				}
				else if(type == terrain::SEGMENT_PIPE)
				{
					if(x > 0 && is_connectable(terrain::SEGMENT_PIPE, currentScene->get_terrain(x-1, y)->type()) && //Links
					   x < 29 && is_connectable(terrain::SEGMENT_PIPE, currentScene->get_terrain(x+1, y)->type())) //Rechts
					{
						[self draw:[NSString stringWithCString:get_path(terrain::SEGMENT_PIPE, terrain::LR).c_str()] at:NSMakePoint(x, y)]; //Gerade (LR)
					}
					else if(y > 0 && is_connectable(terrain::SEGMENT_PIPE, currentScene->get_terrain(x, y-1)->type()) && //Oben
							y < 19 && is_connectable(terrain::SEGMENT_PIPE, currentScene->get_terrain(x, y+1)->type())) //unten
					{
						[self draw:[NSString stringWithCString:get_path(terrain::SEGMENT_PIPE, terrain::UD).c_str()] at:NSMakePoint(x, y)]; //Gerade (UD)
					}
				}
				else if(type == terrain::WRECKAGE)
				{
					if((y > 0 && is_connectable(type, currentScene->get_terrain(x, y-1)->type())) ||
					   (y < 19 && is_connectable(type, currentScene->get_terrain(x, y+1)->type())))
					{
						[self draw:[NSString stringWithCString:get_path(terrain::WRECKAGE, terrain::UD).c_str()] at:NSMakePoint(x, y)];
					}
					else if((x > 0 && is_connectable(type, currentScene->get_terrain(x-1, y)->type())) ||
							(x < 29 && is_connectable(type, currentScene->get_terrain(x+1, y)->type())))
					{
						[self draw:[NSString stringWithCString:get_path(terrain::WRECKAGE, terrain::LR).c_str()] at:NSMakePoint(x, y)];
					}
					else
					{
						//Trümmer
						[self draw:[NSString stringWithCString:get_path(terrain::WRECKAGE, terrain::UNDEF).c_str()] at:NSMakePoint(x, y)];
					}
				}
				else if(type == terrain::MOUNTAIN)
				{
					if(y > 0)
					{
						[self draw:[NSString stringWithCString:get_path(terrain::MOUNTAIN, terrain::UNDEF, terrain::BIG).c_str()] at:NSMakePoint(x, y)];
					}
					else
					{
						[self draw:[NSString stringWithCString:get_path(terrain::MOUNTAIN, terrain::UNDEF, terrain::SMALL).c_str()] at:NSMakePoint(x, y)];
					}
				}
				else if(type == terrain::BEACH)
				{
					
					if(x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::L).c_str()] at:NSMakePoint(x, y)]; //Küste links
					if(x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::R).c_str()] at:NSMakePoint(x, y)]; //Küste rechts
					if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::U).c_str()] at:NSMakePoint(x, y)]; //Küste oben
					if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
						[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::D).c_str()] at:NSMakePoint(x, y)]; //Küste unten
					
					
					if(x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y)->type()))
					{
						if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::UL).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (terrain::UL)
						
						if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::LD).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (LD)
					}
					
					if(x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
					{
						if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::UR).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (UR)
						
						if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::DR).c_str()] at:NSMakePoint(x, y)]; //Nach innen gebogene Küste (DR)
					}
					
					if(y > 0 && y < 19 &&
					   terrain::is_ground(currentScene->get_terrain(x, y-1)->type()) &&
					   terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
					{
						if(x > 0 && terrain::is_ground(currentScene->get_terrain(x-1, y)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::ULD).c_str()] at:NSMakePoint(x, y)]; //Bucht mit Strand
						else if(x < 29 && terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::UDR).c_str()] at:NSMakePoint(x, y)]; //Bucht mit Strand
					}
					
					if(x > 0 && x < 29 &&
					   terrain::is_ground(currentScene->get_terrain(x-1, y)->type()) &&
					   terrain::is_ground(currentScene->get_terrain(x+1, y)->type()))
					{
						if(y > 0 && terrain::is_ground(currentScene->get_terrain(x, y-1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::ULR).c_str()] at:NSMakePoint(x, y)]; //Bucht mit Strand
						else if(y < 19 && terrain::is_ground(currentScene->get_terrain(x, y+1)->type()))
							[self draw:[NSString stringWithCString:get_path(terrain::BEACH, terrain::LDR).c_str()] at:NSMakePoint(x, y)]; //Bucht mit Strand
					}
				}
				else
				{
					[self draw:[NSString stringWithCString:get_path(currentScene->get_terrain(x, y)->type(), terrain::UNDEF, extra).c_str()] at:NSMakePoint(x, y)];
				}
				 */
			}
		}
	}
}

- (void)queueDraw {
	[self setNeedsDisplay:true];
}

@end

