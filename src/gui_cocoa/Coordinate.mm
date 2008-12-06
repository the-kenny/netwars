//
//  Coordinate.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 22.11.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "Coordinate.h"



@implementation Coordinate

#pragma mark "Properties"

@synthesize coord;

@dynamic point;
-(void)setPoint:(NSPoint)p {
	coord.x = p.x;
	coord.y = p.y;
}

-(NSPoint)point {
	return NSMakePoint(coord.x, coord.y);
}

#pragma mark "Initializing and creating"

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
	return [[[Coordinate alloc] initWithCoordinates:x y:y] autorelease];
}

+(Coordinate*)coordinateWithPoint:(NSPoint)p {
	return [[[Coordinate alloc] initWithPoint:p] autorelease];
}

@end
