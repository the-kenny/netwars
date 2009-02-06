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
			}
		}
	}
}

- (void)queueDraw {
	[self setNeedsDisplay:true];
}

@end

