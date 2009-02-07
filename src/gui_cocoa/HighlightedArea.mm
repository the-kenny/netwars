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

@synthesize highlightedArea;

- (NSPoint)toLayerCoordinates:(NSPoint)pos rect:(NSSize)size {	
	NSSize viewSize = [layer bounds].size;
	
	int unflippedX = pos.x*16;
	int unflippedY = pos.y*16;
	
	
	//if(![self isFlipped])
		return NSMakePoint(unflippedX, viewSize.height-unflippedY-16);
	//else
	//	return NSMakePoint(unflippedX, unflippedY+(16-size.height));
}

- (void)draw {
	[layer display];
}

#pragma mark CALayer Delegate
	
	- (void)drawLayer:(CALayer *)layer 
inContext:(CGContextRef)ctx {
	NSGraphicsContext* nscontext =  [NSGraphicsContext graphicsContextWithGraphicsPort:ctx flipped:NO];
	
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nscontext];

	BOOST_FOREACH(const aw::coord& a, highlightedArea)
		[self drawHighlightAt:NSMakePoint(a.x, a.y)];
}

- (void)drawHighlightAt:(NSPoint)pos {
	static const NSString* file = [[NSString stringWithCString:(aw::config().get<std::string>("/config/dirs/pixmaps") + "/misc/range.png").c_str()] retain];

	NSImage* sprite = [[Sprites sharedSprites] getSprite:file];
	[sprite drawAtPoint:[self toLayerCoordinates:pos rect:sprite.size] fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
}

@end
