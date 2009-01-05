#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#include "game/units/unit_base.h"


@interface AnimatableUnit : NSObject {
	__strong aw::unit::ptr unit;
	//Coordinate* positionInViewCoordinates;
	CALayer* layer;
}

@property(assign) aw::unit::ptr unit;
@property(retain) CALayer* layer;

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx;

- (void)draw;

@end
