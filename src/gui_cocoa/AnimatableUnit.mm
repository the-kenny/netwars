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

@synthesize unit;

#pragma mark CALayer Delegate

- (void)drawLayer:(CALayer *)layer 
		inContext:(CGContextRef)ctx {
	NSLog(@"drawLayer:inContext:");
	
	NSGraphicsContext* nscontext =  [NSGraphicsContext graphicsContextWithGraphicsPort:ctx flipped:YES];

	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nscontext];
	
	NSString* path = [NSString stringWithCString:aw::gui::get_path(unit->type(), unit->color()).c_str()];
	NSImage* sprite = [[Sprites sharedSprites] getSprite:path];

	[sprite drawAtPoint:NSMakePoint(.0, .0) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
	
	[NSGraphicsContext restoreGraphicsState];
}

@end
