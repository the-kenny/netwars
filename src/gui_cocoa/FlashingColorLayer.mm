//
//  FlashingColorLayer.mm
//  gui_cocoa
//
//  Created by Moritz Ulrich on 09.10.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FlashingColorLayer.h"


@implementation FlashingColorLayer


- (void)dealloc {
	[layer release];
	[color release];
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

- (void)setColor:(NSColor*)c {
	[color release];
	color = [c retain];
}

- (CALayer*)layer {
	return layer;
}

- (void)draw {
	[layer display];
}

- (void)flash {
	[layer setDelegate:self];
	
	CABasicAnimation* anim = [CABasicAnimation animationWithKeyPath:@"opacity"];
	
	[anim setFromValue:[NSNumber numberWithFloat:0.0]];
	[anim setToValue:[NSNumber numberWithFloat:1.0]];
	
	[anim setDuration:0.1];
	CAMediaTimingFunction* f = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];
	[anim setTimingFunction:f];
	
	[anim setDelegate:self];
	

	[layer addAnimation:anim forKey:@"opacity"];
	
	//[self setOpacity:1.0];
	[self draw];
}

#pragma mark CALayer Delegate

- (void)drawLayer:(CALayer *)layer 
		inContext:(CGContextRef)ctx {
	NSLog(@"FlashAnimation drawLayer:");
	
	
	NSGraphicsContext* nscontext =  [NSGraphicsContext graphicsContextWithGraphicsPort:ctx flipped:NO];
	
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nscontext];

	[color setFill];
	NSRectFill(*(NSRect*)&[[self layer] frame]);
	
	[NSGraphicsContext restoreGraphicsState];
}

#pragma mark CAAnimation Delegate

- (void)animationDidStop:(CAAnimation *)theAnimation finished:(BOOL)flag {
	NSLog(@"FlashAnimation completed");
	[layer removeFromSuperlayer];
	[self release];
}

@end
