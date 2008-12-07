#import <Cocoa/Cocoa.h>

#include "game/coord.h"

@interface Coordinate : NSObject {
	aw::coord coord;
}

@property(readwrite) aw::coord coord;
@property(readwrite) NSPoint point;

-(Coordinate*)initWithCoordinates:(int)x y:(int)y;
-(Coordinate*)initWithPoint:(NSPoint)p;

+(Coordinate*)coordinateWithCoordinates:(int)x y:(int)y;
+(Coordinate*)coordinateWithPoint:(NSPoint)p;

@end
