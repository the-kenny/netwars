//
//  AnimatableUnit.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 13.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "AnimatableUnit.h"
#import "Sprites.h"
#import "Sprites.h"

#include "game/gui/paths.h"
#include "game/units/transporter.h"

@implementation AnimatableUnit

- (void)dealloc {
	[layer release];
	[super dealloc];
}

@dynamic layer;

- (void)setLayer:(CALayer*)l {
	[layer removeFromSuperlayer];
	[layer release];
	
	layer = l;
	[layer retain];
	
	[layer setDelegate:self];
}

- (CALayer*)layer {
	return layer;
}

@dynamic unit;

- (void)setUnit:(aw::unit::ptr)u {
	unit = u;
}

-(aw::unit::ptr)unit {
	return unit;
}

- (void)setPosition:(NSPoint)p {
	NSLog(@"setPosition: %f, %f", p.x, p.y);
	
	[layer setPosition:*(CGPoint*)&p];
}

//Hack
@dynamic currentTerrain;

- (void)setCurrentTerrain:(aw::terrain::ptr)t {
	currentTerrain = t;
}

- (aw::terrain::ptr)currentTerrain {
	return currentTerrain;
}

- (void)moveTo:(NSPoint)p {
	CABasicAnimation* anim = [CABasicAnimation animationWithKeyPath:@"position"];
	[anim setFromValue:[NSValue valueWithPoint:*(NSPoint*)&[layer position]]];
	[anim setToValue:[NSValue valueWithPoint:p]];

	CGPoint q = layer.position;
	
	CGFloat length = (sqrt(((q.x - p.x) * (q.x - p.x)) + ((q.y - p.y) * (q.y - p.y))))/16.0;
	[anim setDuration:1.0/length];
	CAMediaTimingFunction* f = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
	[anim setTimingFunction:f];
	
	//[anim setDelegate:self];
	
	[layer addAnimation:anim forKey:@"move"];
	[self setPosition:p];
}

- (void)draw {
	[layer display];
}

- (void)drawSprite:(NSString*)filename {
	NSImage* sprite = [[Sprites sharedSprites] getSprite:filename];
	[sprite drawAtPoint:NSZeroPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
}

/* 
 UGLY HACK for NSSet in MapView... 
 We just check if self.unit is equal, not self.layer. 
 I hope this won't cause any trouble 
*/
- (BOOL)isEqual:(id)o {
	if(![o isKindOfClass:[self class]])
	   return NO;
	
	AnimatableUnit* au = (AnimatableUnit*)o;
	if([au unit] == [self unit])
	   return YES;
	
	return NO;
}

//The same for hash:
- (NSUInteger)hash {
    NSUInteger hash = [[NSData dataWithBytes:&*unit length:sizeof(*unit)] hash];
    return hash;
}

#pragma mark CALayer Delegate

- (void)drawLayer:(CALayer *)layer 
		inContext:(CGContextRef)ctx {
	using aw::gui::get_path;

	
	NSGraphicsContext* nscontext =  [NSGraphicsContext graphicsContextWithGraphicsPort:ctx flipped:NO];

	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nscontext];
	
	NSString* path = [NSString stringWithCString:get_path(unit->type(), unit->color()).c_str()];
	NSImage* sprite = [[Sprites sharedSprites] getSprite:path];

	[sprite drawAtPoint:NSMakePoint(.0, .0) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
	
	if(unit->moved()) {					 
		NSImage* mask = [sprite copyWithZone:nil];
		
		[mask lockFocus];
		[NSGraphicsContext saveGraphicsState];
		[[NSColor colorWithDeviceRed:0.0 green:0.0 blue:0.0 alpha:1.0] setFill];
		NSRectFillUsingOperation(NSMakeRect(0.0, 0.0, mask.size.width, mask.size.height), NSCompositeSourceIn);
		[NSGraphicsContext restoreGraphicsState];
		[mask unlockFocus];
		
		[mask drawAtPoint:NSZeroPoint
				 fromRect:NSZeroRect 
				operation:NSCompositeSourceOver 
				 fraction:0.4];
		
		[mask release];
	}
	
	int life = unit->hp();
	if(life < 10 && life > 0)
		[self drawSprite:[NSString stringWithCString:get_path(aw::unit::LIVE, life).c_str()]];
	
	if(unit->low_ammo())
		[self drawSprite:[NSString stringWithCString:get_path(aw::unit::LOW_AMMO).c_str()]];
	
	if(unit->low_fuel())
		[self drawSprite:[NSString stringWithCString:get_path(aw::unit::LOW_FUEL).c_str()]];
	
	if(unit->is_hidden())
		[self drawSprite:[NSString stringWithCString:get_path(aw::unit::HIDDEN).c_str()]];
	
	if(unit->is_transporter() && boost::dynamic_pointer_cast<aw::transporter>(unit)->loaded_units_count() > 0)
		[self drawSprite:[NSString stringWithCString:get_path(aw::unit::LOADED).c_str()]];
	
	 if(unit->can_capture())
	 {   
		 if(currentTerrain && currentTerrain->is_building() && boost::dynamic_pointer_cast<aw::building>(currentTerrain)->capture_points() < 20) 
		 {   
			 [self drawSprite:[NSString stringWithCString:get_path(aw::unit::CAPTURE).c_str()]];
		 }   
	 }   
	
	[NSGraphicsContext restoreGraphicsState];
}

#pragma mark CAAnimation Delegate
/*
- (void)animationDidStop:(CAAnimation *)theAnimation finished:(BOOL)flag {
	[self draw];
}
*/

@end
