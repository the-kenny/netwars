//
//  Coordinate.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 22.11.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "game/coord.h"

@interface Coordinate : NSObject {
	aw::coord coord;
}

@property(readwrite) aw::coord coord;

-(Coordinate*)initWithCoordinates:(int)x y:(int)y;
-(Coordinate*)initWithPoint:(NSPoint)p;

+(Coordinate*)coordinateWithCoordinates:(int)x y:(int)y;
+(Coordinate*)coordinateWithPoint:(NSPoint)p;

@end
