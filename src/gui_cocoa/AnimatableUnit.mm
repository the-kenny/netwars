//
//  AnimatableUnit.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 13.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "AnimatableUnit.h"
#import "Sprites.h"

#include "game/gui/paths.h"

@implementation AnimatableUnit

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

- (void)draw {
	[layer display];
}

#pragma mark CALayer Delegate

- (void)drawLayer:(CALayer *)layer 
		inContext:(CGContextRef)ctx {
	//NSLog(@"drawLayer:inContext:");
	
	NSGraphicsContext* nscontext =  [NSGraphicsContext graphicsContextWithGraphicsPort:ctx flipped:NO];

	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nscontext];
	
	NSString* path = [NSString stringWithCString:aw::gui::get_path(unit->type(), unit->color()).c_str()];
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
	
	[NSGraphicsContext restoreGraphicsState];
}

@end
