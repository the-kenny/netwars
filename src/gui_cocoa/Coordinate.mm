//
//  Coordinate.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 22.11.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "Coordinate.h"


@implementation Coordinate
-(Coordinate*)initWithCoordinates:(int)xc y:(int)yc {
	coord.x = xc;
	coord.y = yc;
	return self;
}

-(Coordinate*)initWithPoint:(NSPoint)p {
	coord.x = p.x;
	coord.y = p.y;
	return self;
}
+(Coordinate*)coordinateWithCoordinates:(int)x y:(int)y {
	return [[Coordinate alloc] initWithCoordinates:x y:y];
}

+(Coordinate*)coordinateWithPoint:(NSPoint)p {
	return [[Coordinate alloc] initWithPoint:p];
}

@synthesize coord;

@end
