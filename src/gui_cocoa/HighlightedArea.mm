//
//  HighlightedArea.mm
//  gui_cocoa
//
//  Created by Moritz Ulrich on 07.02.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "HighlightedArea.h"

#import "Sprites.h"

#include "game/config.h"

@implementation HighlightedArea

- (id)init {
	self = [super init];
	
	highlightedArea = aw::area();
	
	return self;
}

- (id)initWithSprite:(NSImage*)s {
	[self init];
	
	sprite = s;
	[sprite retain];
	
	return self;
}

- (void)dealloc {
	[sprite release];
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

@dynamic highlightedArea;

- (void)setHighlightedArea:(aw::area)a {
	highlightedArea = a;
}

-(aw::area)highlightedArea {
	return highlightedArea;
}

- (NSPoint)toLayerCoordinates:(NSPoint)pos rect:(NSSize)size {	
	CGSize viewSize = [layer bounds].size;
	
	int unflippedX = pos.x*16;
	int unflippedY = pos.y*16;
	
	
	return NSMakePoint(unflippedX, viewSize.height-unflippedY-16);
}

- (void)draw {
	[layer setNeedsDisplay];
}

#pragma mark CALayer Delegate

- (void)drawLayer:(CALayer *)layer 
		inContext:(CGContextRef)ctx {
	NSGraphicsContext* nscontext =  [NSGraphicsContext graphicsContextWithGraphicsPort:ctx flipped:NO];
	
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nscontext];
	
	BOOST_FOREACH(aw::coord a, highlightedArea) 
		[self drawHighlightAt:NSMakePoint(a.x, a.y)];
	
	[NSGraphicsContext restoreGraphicsState];
}

- (void)drawHighlightAt:(NSPoint)pos {
	if(sprite == nil) {
		sprite = [[[Sprites sharedSprites] 
				   getSprite:[NSString stringWithCString:(aw::config().get<std::string>("/config/dirs/pixmaps") + "/misc/range.png").c_str()]]
				  retain];
	}
	
	[sprite drawAtPoint:[self toLayerCoordinates:pos rect:sprite.size] fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
}

@end
